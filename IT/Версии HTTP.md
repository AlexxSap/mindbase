
HTTP/3 отличается от HTTP/1.1 и HTTP/2 несколькими ключевыми аспектами:

### 1. Транспортный протокол:

🔸 HTTP/1.1 и HTTP/2: оба используют TCP для передачи данных. TCP обеспечивает надежную доставку, но может страдать от задержек при потере пакетов из-за необходимости восстановления всего потока данных.
🔸 HTTP/3: использует протокол QUIC (Quick UDP Internet Connections), который построен поверх UDP. QUIC обеспечивает быструю и надежную доставку данных с меньшими задержками, т.к. каждый пакет обрабатывается независимо.

### 2. Установка соединения:
🔸 HTTP/1.1 и HTTP/2: для установки соединения требуется несколько этапов, включая установку TCP соединения и выполнение TLS рукопожатия.
🔸 HTTP/3: снижает задержки при установке соединения за счет одновременного выполнения рукопожатия QUIC и TLS, что позволяет начать передачу данных быстрее.

### 3. Управление задержками и потерями пакетов:
🔸 HTTP/1.1 и HTTP/2: при потере пакета в TCP соединении, весь поток данных останавливается до тех пор, пока потерянный пакет не будет восстановлен, что увеличивает задержки.
🔸 HTTP/3: QUIC минимизирует влияние потери пакетов, т.к. каждый поток данных внутри соединения обрабатывается независимо. Это означает, что потеря одного пакета не блокирует другие потоки данных.

### 4. Мультиплексирование:
🔸 HTTP/1.1: не поддерживает мультиплексирование, каждый запрос требует отдельного соединения.
🔸 HTTP/2: поддерживает мультиплексирование, позволяя отправлять несколько запросов и ответов через одно соединение.
🔸 HTTP/3: также поддерживает мультиплексирование, но делает это более эффективно благодаря особенностям QUIC.

Подробнее:
🔗 Cloudflare: What is HTTP/3? (https://www.cloudflare.com/learning/performance/what-is-http3/)
🔗 Google: HTTP/3 Explained (https://developers.google.com/web/fundamentals/performance/http3)