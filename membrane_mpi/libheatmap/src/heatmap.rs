use crate::*;
use std::ptr::null_mut;

enum HeatmapColorschemeChoice {
    Static(*const heatmap_colorscheme_t),
    Dynamic(*mut heatmap_colorscheme_t),
}

impl HeatmapColorschemeChoice {
    fn as_ptr(&self) -> *const heatmap_colorscheme_t {
        match self {
            HeatmapColorschemeChoice::Static(x) => x.clone(),
            HeatmapColorschemeChoice::Dynamic(x) => x.clone(),
        }
    }
}

pub struct HeatmapColorscheme {
    inner: HeatmapColorschemeChoice,
}

pub struct HeatmapStamp {
    inner: *mut heatmap_stamp_t,
}

pub struct Heatmap {
    inner: *mut heatmap_t,
}

impl HeatmapColorscheme {
    pub fn load(colors: &[u8]) -> Self {
        Self {
            inner: HeatmapColorschemeChoice::Dynamic(unsafe {
                heatmap_colorscheme_load(colors.as_ptr(), colors.len())
            }),
        }
    }
}

impl Drop for HeatmapColorscheme {
    fn drop(&mut self) {
        match self.inner {
            HeatmapColorschemeChoice::Static(_) => {}
            HeatmapColorschemeChoice::Dynamic(x) => unsafe { heatmap_colorscheme_free(x) },
        }
    }
}

impl Default for HeatmapColorscheme {
    fn default() -> Self {
        Self {
            inner: HeatmapColorschemeChoice::Static(unsafe { heatmap_cs_default }),
        }
    }
}

impl HeatmapStamp {
    pub fn gen(radius: u32) -> Self {
        Self {
            inner: unsafe { heatmap_stamp_gen(radius) },
        }
    }

    // pub fn gen_nonlinear<F>(radius: u32, dist_shape: F) -> Self
    // where
    //     F: Fn(f32) -> f32,
    // {
    //     Self {
    //         inner: unsafe { heatmap_stamp_gen_nonlinear(radius, dist_shape) },
    //     }
    // }
}

impl Drop for HeatmapStamp {
    fn drop(&mut self) {
        unsafe { heatmap_stamp_free(self.inner) }
    }
}

impl Heatmap {
    pub fn new(width: u32, height: u32) -> Self {
        Self {
            inner: unsafe { heatmap_new(width, height) },
        }
    }

    pub fn get_max(&self) -> f32 {
        unsafe { (&*self.inner).max }
    }

    pub fn set_max(&mut self, value: f32) {
        unsafe { (&mut *self.inner).max = value }
    }

    pub fn add_point(&mut self, x: u32, y: u32) {
        unsafe { heatmap_add_point(self.inner, x, y) };
    }

    pub fn add_point_with_stamp(&mut self, x: u32, y: u32, stamp: &HeatmapStamp) {
        unsafe { heatmap_add_point_with_stamp(self.inner, x, y, stamp.inner) }
    }

    pub fn add_weighted_point(&mut self, x: u32, y: u32, weight: f32) {
        unsafe { heatmap_add_weighted_point(self.inner, x, y, weight) }
    }

    pub fn add_weighted_point_with_stamp(
        &mut self,
        x: u32,
        y: u32,
        weight: f32,
        stamp: &HeatmapStamp,
    ) {
        unsafe { heatmap_add_weighted_point_with_stamp(self.inner, x, y, weight, stamp.inner) }
    }

    pub fn render_default(&self) -> &mut [u8] {
        let this = unsafe { &*self.inner };
        let width = this.w as usize;
        let height = this.h as usize;
        let data = unsafe { heatmap_render_default_to(self.inner, null_mut()) };
        unsafe {
            std::slice::from_raw_parts_mut(data, 4 * width * height * std::mem::size_of::<u8>())
        }
    }

    pub fn render(&self, colorscheme: &HeatmapColorscheme) -> &mut [u8] {
        let this = unsafe { &*self.inner };
        let width = this.w as usize;
        let height = this.h as usize;
        let data = unsafe { heatmap_render_to(self.inner, colorscheme.inner.as_ptr(), null_mut()) };
        unsafe {
            std::slice::from_raw_parts_mut(data, 4 * width * height * std::mem::size_of::<u8>())
        }
    }

    pub fn render_saturated(&self, colorscheme: &HeatmapColorscheme, saturation: f32) -> &mut [u8] {
        let this = unsafe { &*self.inner };
        let width = this.w as usize;
        let height = this.h as usize;
        let data = unsafe {
            heatmap_render_saturated_to(
                self.inner,
                colorscheme.inner.as_ptr(),
                saturation,
                null_mut(),
            )
        };
        unsafe {
            std::slice::from_raw_parts_mut(data, 4 * width * height * std::mem::size_of::<u8>())
        }
    }
}

impl Drop for Heatmap {
    fn drop(&mut self) {
        unsafe { heatmap_free(self.inner) };
    }
}
