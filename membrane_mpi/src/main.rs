use std::fs::File;

use config::Config;
use indicatif::ProgressIterator;
use libheatmap::heatmap::Heatmap;
use matrix::Matrix;
use mpi::point_to_point as p2p;
use mpi::traits::*;
use mpi::Threading;

use crate::draw::HeatmapMatrixDrawer;

extern crate mpi;

mod config;
mod draw;
mod matrix;

fn external(x: f64, y: f64, t: f64, config: &Config) -> f64 {
    if (x - config.width as f64 / 2.0).abs() < 30.0 && (y - config.height as f64 / 2.0).abs() < 10.0
    {
        if 2.0 < t && t < 50.0 {
            return config.external_weight * (x * y + t).cos();
        }
    }
    return 0.;
}

fn explicit_solve(
    x: usize,
    y: usize,
    t: u32,
    curr: &Matrix<f64>,
    next: &mut Matrix<f64>,
    prev: &Matrix<f64>,
    config: &Config,
) {
    let dx = 1.;
    let dy = 1.;
    let dt: f64 = 0.1;

    let a = curr[[x, y]];
    let b = curr[[x - 1, y]];
    let c = curr[[x, y - 1]];
    let d = curr[[x + 1, y]];
    let e = curr[[x, y + 1]];

    let dzx = (d - 2. * a + b) / dx / dx;
    let dzy = (e - 2. * a + c) / dy / dy;
    let dzxy =
        config.parameters.a.powi(2) * (dzx + dzy) + external(x as f64, y as f64, t as f64, config);

    let ap = prev[[x, y]];
    let an = &mut next[[x, y]];

    *an = (-dt.powi(2) * dzxy + ap + a) / 2.;
}

fn main() {
    std::thread::sleep(std::time::Duration::from_secs(20));

    let config = Config::default();
    let num_points = config.width * config.height;

    let universe = mpi::initialize_with_threading(if config.use_threads {
        Threading::Multiple
    } else {
        Threading::Single
    })
    .map(|x| x.0)
    .unwrap();

    let world = universe.world();
    let threads = world.size() as usize;
    let rank = world.rank();
    let root = world.process_at_rank(0);

    let mut current = Matrix::new(config.width, config.height);
    let mut next = Matrix::new(config.width, config.height);
    let mut prev = Matrix::new(config.width, config.height);
    let mut global = Matrix::new(config.width, config.height);

    let mut frames = Vec::<Heatmap>::with_capacity(config.time_layers as usize);
    let mut last_max = 0.0;

    let offset = num_points / threads;
    assert_eq!(num_points % threads, 0);

    let start = offset * rank as usize;
    let end = offset * (rank + 1) as usize;
    let size = offset;

    let mut top_buf = Vec::with_capacity(size);
    let mut down_buf = Vec::with_capacity(size);

    for t in (0..config.time_layers) {
        for i in start..end {
            let x = i % config.width;
            let y = i / config.width;

            if x == 0 || y == 0 || x == config.width - 1 || y == config.height - 1 {
                next[[x, y]] = 0.0;
            } else {
                explicit_solve(x, y, t, &current, &mut next, &prev, &config);
            }
        }

        prev.data.swap_with_slice(&mut current.data);
        current.data.swap_with_slice(&mut next.data);

        if rank != 0 {
            let prev_process = world.process_at_rank(rank - 1);
            let curr_process = world.process_at_rank(rank);
            p2p::send_receive_into(
                &current.data[start - size..start],
                &prev_process,
                &mut top_buf[..],
                &prev_process,
            );

            current.data[start - size..start].swap_with_slice(&mut top_buf[..]);
        }
        if rank != (threads - 1) as i32 {
            let next_process = world.process_at_rank(rank + 1);
            let curr_process = world.process_at_rank(rank);
            p2p::send_receive_into(
                &current.data[end..end + size],
                &next_process,
                &mut down_buf[..],
                &next_process,
            );

            current.data[end..end + size].swap_with_slice(&mut down_buf[..]);
        }

        if rank != 0 {
            root.gather_into_root(&prev.data[start..end], &mut global.data[start..end]);
        } else {
            global.data[..end].clone_from_slice(&prev.data[..end]);
        }

        if config.generate_image && rank == 0 {
            frames.push(HeatmapMatrixDrawer::new_f64(&global).into_heatmap());
            let frame = frames.get_mut(t as usize).unwrap();
            if frame.get_max() > last_max {
                last_max = frame.get_max();
            }

            frame.set_max(last_max);
        }
    }

    world.barrier();

    if config.generate_image && rank == 0 {
        let gif_file = File::create("output.gif").unwrap();
        let mut encoder =
            gif::Encoder::new(gif_file, config.width as u16, config.height as u16, &[]).unwrap();

        let last_frame = frames.pop().unwrap();

        for frame in frames.into_iter() {
            let image = frame.render_default();
            let frame = gif::Frame::from_rgba(config.width as u16, config.height as u16, image);

            encoder.write_frame(&frame).unwrap();
        }

        let image = last_frame.render_default();

        lodepng::encode32_file("output.png", image, config.width, config.height)
            .expect("Cannot encode image");
    }
}
