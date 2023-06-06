use std::ops::{Index, IndexMut};

use stable_id_traits::CastUsize;

#[derive(Clone)]
pub struct Matrix<T> {
    pub data: Vec<T>,
    pub width: usize,
    pub height: usize,
}

impl<T> Matrix<T> {
    pub fn new(width: usize, height: usize) -> Matrix<T>
    where
        T: Default,
    {
        let width = width;
        let height = height;
        let data = (0..width * height).map(|_| T::default()).collect();

        Self {
            data,
            width,
            height,
        }
    }

    pub fn map<V, F>(&self, f: F) -> Matrix<V>
    where
        F: Fn(&T) -> V,
    {
        Matrix {
            data: self.data.iter().map(f).collect(),
            width: self.width,
            height: self.height,
        }
    }
}

impl<T, P: CastUsize> Index<[P; 2]> for Matrix<T> {
    type Output = T;

    fn index(&self, index: [P; 2]) -> &Self::Output {
        let [x, y] = index.map(|x| x.cast_to());
        &self.data[x * self.width + y]
    }
}

impl<T, P: CastUsize> IndexMut<[P; 2]> for Matrix<T> {
    fn index_mut(&mut self, index: [P; 2]) -> &mut Self::Output {
        let [x, y] = index.map(|x| x.cast_to());
        &mut self.data[x * self.width + y]
    }
}
