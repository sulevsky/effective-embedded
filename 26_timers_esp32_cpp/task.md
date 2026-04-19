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