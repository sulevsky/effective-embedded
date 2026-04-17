## Завдання 4: Polling + debounce (без interrupts)
+ Прибрати interrupt повністю
+ Опитувати кнопку кожні 5–10 мс
+ Реалізувати debounce як машину станів

Очікуваний результат
+ Найстабільніша поведінка
Трохи більша затримка реакції

# State machine
|State\Event|PRESS|RELEASE|TIMEOUT|
|-|-|-|-|
|STATE_IDLE|STATE_PRESSED, Start timer 50 ms|-|-|
|STATE_PRESSED|-|-|STATE_HELD_SHORT, Start timer 20000 ms|
|STATE_HELD_SHORT|-|STATE_RELEASED, short_press_num incr, Start timer 50 ms|STATE_HELD_LONG|
|STATE_HELD_LONG|-|STATE_RELEASED, long_press_num incr, Start timer 50 ms|-|
|STATE_RELEASED|-|-|STATE_IDLE|


 	 	 	 
