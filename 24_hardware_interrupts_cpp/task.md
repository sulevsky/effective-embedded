# Домашнє завдання: Усунення брязкоту контактів кнопки

- Код і результат виконання на GitHub.
- Посилання на репозиторій в LMS.
- Використовувати Arduino Framework або ESP-IDF.

## + Завдання 1: Базова реалізація (без debounce, робили на уроці)
+ Реалізувати GPIO interrupt на FALLING
При кожному interrupt:
+ інкрементувати лічильник
+ Виводити лічильник у лог

Очікуваний результат
+ 1 фізичне натискання -> кілька interrupt
+ Interrupt викликається і при відпусканні кнопки


## + Завдання 2: Software debounce через таймер (time-based)
+ Додати debounce на основі часу:
+ ігнорувати interrupt, якщо з моменту попереднього пройшло < 50 мс
+ Реалізувати поза ISR

Очікуваний результат
+ Менше хибних спрацювань
  Все ще можливі спрацювання при відпусканні

## Завдання 3: Debounce через перевірку рівня (state-based)
Interrupt лише сигналізує “подія”
У loop() / task:
приймати подію лише якщо кнопка досі натиснута
ігнорувати події при відпусканні

Очікуваний результат
Рівно 1 реакція на 1 натискання
Release не викликає дії

## Завдання 4: Polling + debounce (без interrupts)
+ Прибрати interrupt повністю
+ Опитувати кнопку кожні 5–10 мс
+ Реалізувати debounce як машину станів

Очікуваний результат
+ Найстабільніша поведінка
Трохи більша затримка реакції

## Завдання 5: Hardware debounce
Додати RC-фільтр (конденсатор 100n і резистор 100R, підтягуючий незмінний 10k)
Повторити частини 1-4
Порівняти результати з RC і без RC

## Завдання 6: Порівняльна таблиця
|Метод|Кількість хибних спрацювань|Затримка|Складність|Коментар|
|-|-|-|-|-|
|Без debounce|||||||
|Time-based|||||||
|State-based|||||||
|Polling|||||||
|Hardware RC|||||||


 	 	 	 


# State machine
|State\Event|PRESS|RELEASE|TIMEOUT|NONE|
|-|-|-|-|-|
|STATE_IDLE|STATE_PRESSED, Start timer 50 ms|-|-|-|
|STATE_PRESSED|-|-|STATE_HELD_SHORT, Start timer 2000 ms|-|
|STATE_HELD_SHORT|-|STATE_RELEASED, short_press_num incr, Start timer 50 ms|STATE_HELD_LONG|-|
|STATE_HELD_LONG|-|STATE_RELEASED, Start timer 50 ms, long_press_num incr|-|-|
|STATE_RELEASED|-|-|STATE_IDLE|-|


 	 	 	 
