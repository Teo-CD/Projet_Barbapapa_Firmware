use std::fs::File;
use std::fs::write;
use std::env;
use std::io::Read;

fn main() {
    let args: Vec<String> = env::args().collect();

    for arg in &args[1..] {
        println!("Processing : {}", arg);
        if !arg.ends_with(".mono") {
            eprintln!("{} : unsupported file type.", arg);
            continue;
        }
        let file_r = File::open(arg);
        if file_r.is_err() {
            eprintln!("{} : could not open file.", arg);
            continue;
        }
        let mut image_data = vec![];
        if file_r.unwrap().read_to_end(&mut image_data).is_err() {
            eprintln!("{} : could not read file.", arg);
            continue;
        }

        let out_file = arg.replace(".mono", ".bin");
        flip_bits(&mut image_data);
        if write(&out_file, &image_data).is_err() {
            eprintln!("Failed to write bit flipped bitmap to file : {}", out_file);
        }
        println!("Processing successful, flipped bits and wrote to {}", out_file);
    }
}

/// Flips the bits of each individual byte (except zeroes).
/// This would transform 0b11001100 in 0b00110011.
fn flip_bits(data: &mut Vec<u8>) {
    for source_id in 0..data.len() {
        let mut new_int = 0;
        for i in 0..8 {
            if data[source_id] == 0 { break; }
            new_int |= ((data[source_id] & (1 << 7 - i)) >> 7 - i) << i;
        }
        data[source_id] = new_int;
    }
    // for i in 0..data.len() {
    //     print!("{:2X}", data[i]);
    //     if i > 0 && i%16 == 0 { println!(); }
    // }
    // println!();
}
