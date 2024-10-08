
[CAP-теорема простым, доступным языком](https://habr.com/ru/post/130577/)
[Недопонимание CAP-теоремы](https://habr.com/ru/articles/136305/)

CAP-теорема утверждает, что в любой реализации распределённых вычислений можно обеспечить не более двух из трёх следующих свойств:
1. `Согласованность данных` (Consistency): как только мы успешно записали данные в наше распределенное хранилище, любой клиент при запросе получит актуальные данные.
2. `Доступность` (Availability): в любой момент клиент может получить данные из нашего хранилища или получить ответ об их отсутствии, если их никто еще не сохранял.
3. `Устойчивость к разделению` (Partition Tolerance): потеря сообщений между компонентами системы (возможно даже потеря всех сообщений) не влияет на работоспособность системы.

### Распределенные системы и CAP-теорема

Распределённые системы, в зависимости от поддерживаемых двух из трёх свойств CAP-теоремы, делятся на три категории:

#### CA (Consistency + Availability):
🔸 Во всех узлах данные согласованы и обеспечена доступность. При этом система жертвует устойчивостью к разделению.
🔸 Пример: системы, использующие репликацию данных и требуют синхронного подтверждения от всех узлов (в условиях отсутствия разделений).

#### CP (Consistency + Partition Tolerance):
🔸 В каждый момент система обеспечивает целостный результат и способна функционировать в условиях разделения, но достигает этого в ущерб доступности (может не выдавать отклик на запрос).
🔸 Пример: системы баз данных, такие как HBase, где в условиях разделения данные остаются согласованными, но некоторые части системы могут быть недоступны.

#### AP (Availability + Partition Tolerance):
🔸 Не гарантируется целостность данных, но обеспечивается доступность и устойчивость к разделению.
🔸 Пример: системы, такие как Cassandra, которые обеспечивают доступность и устойчивость к разделению, но могут возвращать устаревшие данные.

