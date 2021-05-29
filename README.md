# STM_Project

Реализуйте часы с помощью встроенного блока часов реального времени (RTC) и двухстрочного символьного дисплея.
Для установки времени используйте кнопки на второй плате, подключенной через интерфейс `RS-485`.
При срабатывании будильника, выдайте прерывистый сигнал на плате с кнопками. 

## Подключение

Контроллер дисплея `HD44780`, формат 16×2, 4 бита.

- EN — PC4
- RS — PB12
- RW — PB3
- D4 — PB4
- D5 — PB5
- D6 — PB6
- D7 — PB7
- подсветка — PC6

## Ссылки

В сети тема разжёвана на каждом углу, но полезно почитать оригинальное описание (особенно инициализацию).

- http://fab.cba.mit.edu/classes/863.06/11.13/44780.pdf

- https://www.sparkfun.com/datasheets/LCD/HD44780.pdf