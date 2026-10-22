//! Reaction workers.
//!
//! Multithreading support for simultaneous reactions.
//!
//! There are 3 steps to any reaction, at least w.r.t. the simulation state:
//! - Picking a valid pair of non-busy programs
//! - Concatenating them, executing them.
//! - Writing the results back into the simulation state.
//!
//! Workers must always pick disjoint pairs of programs w.r.t. other workers,
//! otherwise they risk a data race.  So we need to track the fact that a worker
//! is using some program i.e. that program is busy.

use std::sync::{Condvar, Mutex};

use crate::{
    bf::{ProgramId, Reaction},
    constants::{MUTATION_CHANCE, NUM_PROGRAMS, SIMULATION_ROW_SIZE},
    state::State,
};

/// Perform one complete reaction on two unreserved programs given the current
/// `state` of the simulation.
///
/// This involves reserving two unreserved programs, reacting them, then freeing
/// them.
pub fn work_once(state: &Mutex<State>, changed: &Condvar) {
    let (first, second, mut reaction) = {
        let mut state = state.lock().unwrap();

        // The mutator blocks new reservations until every active reaction finishes.
        while state.mutating {
            state = changed.wait(state).unwrap();
        }

        // If we don't have at least two programs unreserved, we cannot do
        // anything.
        if state.reserved.iter().filter(|&&x| !x).count() < 2 {
            return;
        }

        // Reserve a pair of programs
        let Some((first, second)) =
            choose_unreserved_neighbours(&state.reserved)
        else {
            return;
        };
        state.reserved[first] = true;
        state.reserved[second] = true;

        (
            first,
            second,
            Reaction::new(
                ProgramId::new(first),
                ProgramId::new(second),
                &state.data,
            ),
        )
    };

    reaction.execute();

    {
        let mut state = state.lock().unwrap();
        reaction.finish(&mut state.data);
        state.reserved[first] = false;
        state.reserved[second] = false;
    }

    changed.notify_all();
}

pub fn mutate(state: &Mutex<State>, changed: &Condvar) {
    if MUTATION_CHANCE == 0 {
        return;
    }

    let mut state = state.lock().unwrap();
    state.mutating = true;
    while state.reserved.iter().any(|&busy| busy) {
        state = changed.wait(state).unwrap();
    }

    // No worker can reserve a program until the mutation is complete.
    for cell in &mut state.data {
        if rand::random_range(1..=MUTATION_CHANCE) == 1 {
            *cell = rand::random();
        }
    }

    state.mutating = false;
    drop(state);

    changed.notify_all();
}

/// Choose two unreserved programs randomly given the set of reserved programs
/// `res`.
fn choose_unreserved_pair(res: &[bool; NUM_PROGRAMS]) -> (usize, usize) {
    let mut a = rand::random_range(0..res.len());
    while res[a] {
        a = rand::random_range(0..res.len());
    }

    let mut b = rand::random_range(0..res.len());
    while res[b] || a == b {
        b = rand::random_range(0..res.len());
    }

    (a, b)
}

/// Choose two unreserved neighbouring programs randomly given the set of
/// reserved programs `res`.
fn choose_unreserved_neighbours(
    res: &[bool; NUM_PROGRAMS],
) -> Option<(usize, usize)> {
    let mut a = rand::random_range(0..res.len());
    while res[a] {
        a = rand::random_range(0..res.len());
    }

    let x = a % SIMULATION_ROW_SIZE;
    let y = a / SIMULATION_ROW_SIZE;

    let mut neighbours = [0; 8];
    let mut count = 0;

    for (dx, dy) in [
        (-1, -1),
        (0, -1),
        (1, -1),
        (-1, 0),
        (1, 0),
        (-1, 1),
        (0, 1),
        (1, 1),
    ] {
        if let (Some(nx), Some(ny)) =
            (x.checked_add_signed(dx), y.checked_add_signed(dy))
        {
            let choice = nx + (ny * SIMULATION_ROW_SIZE);
            if nx < SIMULATION_ROW_SIZE
                && ny < SIMULATION_ROW_SIZE
                && !res[choice]
            {
                neighbours[count] = choice;
                count += 1;
            }
        }
    }

    (count > 0).then(|| (a, neighbours[rand::random_range(0..count)]))
}
