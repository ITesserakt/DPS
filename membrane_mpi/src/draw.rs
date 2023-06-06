use std::borrow::Cow;

use libheatmap::heatmap::Heatmap;

use crate::matrix::Matrix;

pub struct HeatmapMatrixDrawer<'a>(Cow<'a, Matrix<f32>>);

impl<'a> HeatmapMatrixDrawer<'a> {
    pub fn new_f64(matrix: &'a Matrix<f64>) -> Self {
        Self(Cow::Owned(matrix.map(|&x| x as f32)))
    }

    pub fn into_heatmap(self) -> Heatmap {
        let mut heatmap = Heatmap::new(self.0.width as u32, self.0.height as u32);

        for x in 0..self.0.width {
            for y in 0..self.0.height {
                heatmap.add_weighted_point(x as u32, y as u32, self.0[[x, y]].abs());
            }
        }

        heatmap
    }
}
