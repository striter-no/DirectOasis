# Game Network Protocol

Протокол для общения между сервером и клиентом (UDP).

## Структура сообщений

От клиента к серверу:

```plaintext
EndPoint: dir/subdir/path
Data-Type: text/json
Msg-Spec: cli-cli/broadcast
To-Cli-UID: 123456
UID: 123456
Content:
Hello world
```

От сервера к клиенту:

```
StatusCode: 0
Data-Type: text/json
Msg-Spec: cli-cli/broadcast/server
Cli-From: 123456
Content:
...
```


## Сервер

1. У сервера есть несколько end-point'ов для сообщений от клиентов
2. В зависимости от end-point каждое сообщение обрабатывается соответствующе (на усмотрение пользователя)
3. У сервера есть свои заголовки (сообщение от клиента):
   1. **EndPoint**
        
        Спецификация конечного пути на сервере, в формате `dir/subdir/file.ext`

    2. **Data-Type**

        Спецификация отправляемого контента:
        - `text`: обычный текст
        - `json`: текст в формате JSON
    
    3. **Msg-Spec**

        Спецификация вида сообщения:
        - `cli-cli`: клиент-клиент. Должен быть далее указан заголовок `To-Cli-UID` в качестве адресата
        - `broadcast`: отправляет сообщение всем клиентам, подключенные к тому же серверу, что и данный клиент
    
    4. **To-Cli-UID/UID**

        Спецификация *UID* пользователя:
        - `To-Cli`: UID адресата, если сообщение типа `cli-cli`
        - `UID`: UID отправителя. Указывается в любом случае
    
    5. **Content**

        Информация, в формате указанного в заголовке `Data-Type`. Должна быть пустая строка после него и далее сама информация:

        ```
        ...
        Content:
        Lorem ipsum ...
        ```

4. Сообщения от сервера:

    1. **StatusCode**

        Какой статус у ответа, т.е. как обработался запрос

    2. **Data-Type**

        Тип информации в ответе

    3. **Msg-Spec**

        Какой тип у сообщения (характеристика сообщения, т.е. откуда оно появилось)

        Если тип сообщения `server`, то значит сообщение было отправлено сервером.

    4. **Cli-From**

        Если `Msg-Spec` равен `cli-cli` или `broadcast`, то это UID отправителя

    5. **Content**

        Информация, в формате указанного в заголовке `Data-Type`

## Коды статусов

1.  0  - Success
2.  1  - Not authorized
3.  2  - Internal Server Error
4.  3  - Endpoint path is not found
5.  4  - Forbiden
6.  5  - The action is not doable in this context
7.  6  - Invalid message format
8.  7  - Invalid value
9.  8  - Invalid data type
10. 9  - Addressee does not exist
11. 10 - Server is empty