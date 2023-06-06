pub struct Parameters {
    pub a: f64,
}

pub struct Config {
    pub generate_image: bool,
    pub height: usize,
    pub width: usize,
    pub time_layers: u32,
    pub external_weight: f64,
    pub parameters: Parameters,
    pub use_threads: bool,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            generate_image: true,
            height: 300,
            width: 300,
            time_layers: 60,
            external_weight: 1.,
            parameters: Parameters { a: 0.57 },
            use_threads: true,
        }
    }
}
