## Задача

+ Реалізувати керування витяжним вентилятором, який кожну годину вмикається на 15 хвилин, після чого автоматично вимикається.

+ Вентилятор = двигун || реле
+ Таймери мають працювати незалежно від коду в циклі
+ Не використовувати delay() для реалізації завдання
+ Цикл повторюється без участі loop() / task
+ Час роботи і період задаються змінними або макросами
Для налагодження можна:

+ замінити 1 годину на кілька секунд
+ 15 хв замінити на пропорційно менший відрізок часу
Опційно

- Watchdog
+ Логування станів
+ Захист від повторного запуску
+ ЛЕД індикатори, що дублюють поточний стан
 

## Notes
- https://github.com/espressif/esp-idf/blob/master/examples/peripherals/timer_group/gptimer/main/gptimer_example_main.c
- https://github.com/espressif/esp-idf/blob/master/examples/system/esp_timer/main/esp_timer_example_main.c

## TODO 
+ non voloatile storage
Bug: Flash write every second (line 86)
storage.save() on every tick = 86,400 writes/day. At ~100k write cycles per sector, the NVS partition wears out in ~1.2 days. Save only on state transitions:

+ alternative signals
static COUNTER_MINUTES: Mutex<CriticalSectionRawMutex, RefCell<u32>> = Mutex::new(RefCell::new(0));
static COUNTER_MINUTES: AtomicU32 = AtomicU32::new(0);

