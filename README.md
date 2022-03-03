# IRC

Запуск сервера:

```
make
./ircserv <port> <password>
```

Запуск клиента:

 -   `nc localhost <port>`
 -   Графический клиент Pidgin (Linux)
 -   Графический клиент Adium (MacOS)
 
PASS-сообщение не должно быть введено клиентом в последнюю очередь. Если регистрация прошла успешна, клиенту отправляется MOTD (message of the day).
 Обязательные условия для регистрации пользователя:
-   PASS-сообщение
-   NICK-сообщение
-   USER-сообщение


---
Cервер не поддерживает коммуникацию _сервер-сервер_. Доступные команды, в соответствии со стандартом [rfc1459](https://datatracker.ietf.org/doc/html/rfc1459):

PASS  
NICK
USER
OPER
QUIT
PRIVMSG
AWAY
NOTICE
WHO
WHOIS
WHOWAS
MODE
TOPIC
JOIN
INVITE
KICK
PART
NAMES
LIST
WALLOPS
PING / PONG
ISON
USERHOST
VERSION
INFO
ADMIN
TIME

