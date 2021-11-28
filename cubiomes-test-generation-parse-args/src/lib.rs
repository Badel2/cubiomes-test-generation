use std::ffi::CStr;
use std::ffi::CString;
use std::ffi::OsString;
use std::mem;
use std::os::raw::c_char;
use std::path::PathBuf;
use structopt::StructOpt;

#[derive(Debug, StructOpt)]
#[structopt(
    name = "cubiomes-test-generation",
    about = "Test cubiomes against real minecraft worlds"
)]
pub struct Opt {
    #[structopt(long)]
    mc_version: String,
    #[structopt(long, parse(from_os_str))]
    input_zip: PathBuf,
    #[structopt(long)]
    save_img: bool,
    #[structopt(long)]
    y_level: Option<i64>,
}

/// Parsed arguments. You need to manually call `free_args` in order to free the memory.
#[repr(C)]
pub struct Args {
    mc_version: *mut c_char,
    input_zip: *mut c_char,
    save_img: bool,
    y_level: OptionI64,
}

#[repr(C)]
pub struct OptionI64 {
    is_some: bool,
    value: i64,
}

/// Parse args into `parsed_args`. Returns 0 on success, non-0 on error, and exits the program if
/// the arguments are not valid showing a small help in the process.
///
/// # Safety
///
/// `argc` and `argv` should follow the usual conventions.
/// `parsed_args` must be a valid pointer.
#[no_mangle]
pub unsafe extern "C" fn parse_args(
    argc: usize,
    argv: *const *const c_char,
    parsed_args: *mut Args,
) -> i32 {
    // Convert (argc, argv) into Iterator<Item = OsString>
    let it = (0..argc).map(|i| {
        let cs: *const c_char = unsafe { *argv.add(i) };
        let cstr = unsafe { CStr::from_ptr(cs) };
        let rstr = cstr.to_string_lossy();
        OsString::from(rstr.to_string())
    });

    // Parse args into Opt using the iterator
    let opt = Opt::from_iter(it);

    // Convert Opt (Rust struct) into Args (C string)
    let Opt {
        mc_version,
        input_zip,
        save_img,
        y_level,
    } = opt;

    let mc_version = CString::new(mc_version).unwrap().into_raw();
    let input_zip = CString::new(input_zip.to_str().unwrap())
        .unwrap()
        .into_raw();
    let y_level = OptionI64 { is_some: y_level.is_some(), value: y_level.unwrap_or(0) };

    let args = Args {
        mc_version,
        input_zip,
        save_img,
        y_level,
    };

    unsafe {
        *parsed_args = args;
    }

    0
}

/// Free memory allocated by `parse_args()`.
#[no_mangle]
pub extern "C" fn free_args(parsed_args: Args) {
    let Args {
        mc_version,
        input_zip,
        save_img: _,
        y_level: _,
    } = parsed_args;

    unsafe {
        mem::drop(CString::from_raw(mc_version));
        mem::drop(CString::from_raw(input_zip));
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}
