#![no_std]
#![no_main]

use core::f32::consts::PI;

use cortex_m::{asm, delay::Delay};
use defmt::{info, println};

use defmt_rtt as _;
use embedded_hal::digital::OutputPin;
use fugit::{Duration, Rate};
use panic_probe as _;
use stm32f4xx_hal::{
    dac::{DacOut, DacPin},
    dwt::{Instant, MonoTimer},
    gpio::GpioExt,
    prelude::*,
    rcc::Rcc,
};

const SHOULD_PLAY_DAC: bool = true;
const SHOULD_PLAY_SQUARE: bool = !SHOULD_PLAY_DAC;

struct Note {
    name: &'static str,
    frequency: Rate<u32, 1, 1>,
}
impl Note {
    fn new(name: &'static str, frequency: Rate<u32, 1, 1>) -> Self {
        Self { name, frequency }
    }
}

#[cortex_m_rt::entry]
fn main() -> ! {
    let note_names_to_frequency: [Note; _] = [
        Note::new("C4", 262.Hz()),
        Note::new("D4", 294.Hz()),
        Note::new("E4", 330.Hz()),
        Note::new("F4", 349.Hz()),
        Note::new("G4", 392.Hz()),
        Note::new("A4", 440.Hz()),
        Note::new("B4", 493.Hz()),
        Note::new("C5", 523.Hz()),
        Note::new("PAUSE", 1.Hz()),
    ];
    // E E E | E E E | E G C D E
    let jingle_bells: [Note; _] = [
        Note::new("E4", 330.Hz()),
        Note::new("E4", 330.Hz()),
        Note::new("E4", 330.Hz()),
        Note::new("PAUSE", 1.Hz()),
        Note::new("E4", 330.Hz()),
        Note::new("E4", 330.Hz()),
        Note::new("E4", 330.Hz()),
        Note::new("PAUSE", 1.Hz()),
        Note::new("E4", 330.Hz()),
        Note::new("G4", 392.Hz()),
        Note::new("C4", 262.Hz()),
        Note::new("D4", 294.Hz()),
        Note::new("E4", 330.Hz()),
    ];

    info!("Starting initialization");
    let cp = cortex_m::Peripherals::take().unwrap();
    let dp = stm32f4xx_hal::pac::Peripherals::take().unwrap();
    let mut rcc = dp.RCC.constrain();
    let gpioa = dp.GPIOA.split(&mut rcc);
    let mut square_wave_pin = gpioa.pa9.into_push_pull_output().erase();

    let mut delay = Delay::new(cp.SYST, rcc.clocks.sysclk().raw());
    let dac_pin = gpioa.pa5;
    let mut dac_out = stm32f4xx_hal::dac::dac(dp.DAC, dac_pin.into_analog(), &mut rcc);
    dac_out.enable();

    let mono_timer = MonoTimer::new(cp.DWT, cp.DCB, &rcc.clocks);
    info!("Finished initialization");
    loop {
        info!("Playing all notes");
        // all
        for Note { name, frequency } in &note_names_to_frequency {
            if SHOULD_PLAY_DAC {
                info!("Playing {} in DAC", name);
                play_analog(frequency, &mut dac_out, 1u32.secs(), &mono_timer, &mut rcc);
                delay.delay_ms(500);
            }
            if SHOULD_PLAY_SQUARE {
                info!("Playing {} in SQUARE", name);

                play_square(
                    frequency,
                    &mut square_wave_pin,
                    1u32.secs(),
                    &mono_timer,
                    &mut rcc,
                );
                delay.delay_ms(500);
            }
        }

        // bells
        info!("Playing all Jingle Bells");
        for Note { name, frequency } in &jingle_bells {
            if SHOULD_PLAY_DAC {
                info!("Playing {} in DAC", name);
                play_analog(
                    frequency,
                    &mut dac_out,
                    200u32.millis(),
                    &mono_timer,
                    &mut rcc,
                );
                delay.delay_ms(100);
            }
            if SHOULD_PLAY_SQUARE {
                info!("Playing {} in SQUARE", name);

                play_square(
                    frequency,
                    &mut square_wave_pin,
                    200u32.millis(),
                    &mono_timer,
                    &mut rcc,
                );
                delay.delay_ms(100);
            }
        }

        info!("Done restaring in 10 seconds");
        delay.delay_ms(10_000);
    }
}

const SIN_FUNCTION_RESOLUTION: usize = 16;

const SIN_DAC_VALUE: [u16; SIN_FUNCTION_RESOLUTION] = [
    2048, 2855, 3532, 3968, 4093, 3886, 3382, 2661, 1840, 1054, 428, 65, 23, 309, 878, 1635,
];

#[allow(dead_code)]
fn step_to_radians(step: u32, num_of_steps: u32) -> f32 {
    ((step as f32) * 2.0 * PI) / ((num_of_steps - 1) as f32)
}

#[allow(dead_code)]
fn to_dac_range(x: f32) -> u16 {
    // sin - [-1,1]
    // shift 1 up - [0,2]
    // scale to 4096, mult 2048 - [0,4096]
    // to u16
    // trim 4096 not included - [0,4095]
    (((x + 1.0) * 2048.0) as u16) % 4096
}

fn play_analog(
    frequency: &Rate<u32, 1, 1>,
    dac_out: &mut impl DacOut<u16>,
    duration: Duration<u32, 1, 1_000_000>,
    mono_timer: &MonoTimer,
    rcc: &mut Rcc,
) {
    let play_start = mono_timer.now();

    if frequency.raw() == 1 {
        dac_out.set_value(0);
        while elapsed(play_start, rcc) < duration {
            //busy wait for full period
            asm::nop();
        }
    }

    let period_duration: Duration<u32, 1, 1_000_000> = frequency.into_duration();
    let step_duration: Duration<u32, 1, 1_000_000> =
        period_duration / (SIN_FUNCTION_RESOLUTION as u32);
    let mut num_periods = 0;
    while elapsed(play_start, rcc) < duration {
        let period_start = mono_timer.now();
        let mut cummulative_step_duration = step_duration;
        for step in 0..SIN_FUNCTION_RESOLUTION {
            // let x = step_to_radians(step, SIN_FUNCTION_RESOLUTION);
            // let value = libm::sinf(x);
            // let dac_value = to_dac_range(value);
            // dac_out.set_value(dac_value);
            let dac_value = SIN_DAC_VALUE[step];
            dac_out.set_value(dac_value);
            while elapsed(period_start, rcc) < cummulative_step_duration {
                asm::nop();
            }
            cummulative_step_duration += step_duration;
        }
        num_periods += 1;
    }
    dac_out.set_value(0);
    println!("num_periods {}", num_periods);
}

fn play_square(
    frequency: &Rate<u32, 1, 1>,
    pin: &mut impl OutputPin,
    duration: Duration<u32, 1, 1_000_000>,
    mono_timer: &MonoTimer,
    rcc: &mut Rcc,
) {
    let play_start = mono_timer.now();

    if frequency.raw() == 1 {
        pin.set_low().unwrap();
        while elapsed(play_start, rcc) < duration {
            //busy wait for full period
            asm::nop();
        }
    }

    let period_duration: Duration<u32, 1, 1_000_000> = frequency.into_duration();
    let half_period_duration: Duration<u32, 1, 1_000_000> = period_duration / 2;
    while elapsed(play_start, rcc) < duration {
        let period_start = mono_timer.now();
        pin.set_low().unwrap();

        while elapsed(period_start, rcc) < half_period_duration {
            //busy wait for half period
            asm::nop();
        }
        pin.set_high().unwrap();
        while elapsed(period_start, rcc) < period_duration {
            //busy wait for full period
            asm::nop();
        }
        pin.set_low().unwrap();
    }
}

fn elapsed(start: Instant, rcc: &mut Rcc) -> Duration<u32, 1, 1_000_000> {
    (start.elapsed() / (rcc.clocks.sysclk().raw() / 1000000)).micros()
}
