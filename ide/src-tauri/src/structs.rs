use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Clone)]
pub struct Configs {
    pub bin_path: String,
    pub file_path: String,
}
