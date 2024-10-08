# Простой полнотекстовый файловый поисковик

В данной работе реализован проcтой [полнотекстовый поиск](https://en.wikipedia.org/wiki/Full-text_search) над файлами.

Решение подобных задач предполагает подготовку [поискового индекса](https://en.wikipedia.org/wiki/Search_engine_indexing) (набора файлов с различными структурами данных) и поиска по полученным файлам.

Конкретно здесь реализован один из самых простых вариантов поискового индекса - [инвертировнный индекс](https://en.wikipedia.org/wiki/Inverted_index).

Задача полнотекстового поиска так же подразумевает решение проблемы ранжирования (упорядочевания найденных по запросу документов). Здесь реализована функция ранжироавния [BM25](https://en.wikipedia.org/wiki/Okapi_BM25), а в качестве фактора для нее был взят [TF-IDF](https://en.wikipedia.org/wiki/Tf%E2%80%93idf).

Таким образом реализовано два приложения - индексатор (подготовка поискового индекса) и поиск (программа, осуществляющая непосредственно поиск по построенному индексу).

## Описание индексирующей программы

- Консольное приложение.
- Обход всех файлов в директориях производится рекурсивно.
- Проверка слов строгая - приведение слов к [нормальной форме](https://ru.wikipedia.org/wiki/%D0%9B%D0%B5%D0%BC%D0%BC%D0%B0%D1%82%D0%B8%D0%B7%D0%B0%D1%86%D0%B8%D1%8F) не производится.

## Описание поисковой программы

- Консольное приложение, взаимодействие с пользователем через стандартные потоки ввода и вывода.
- Осуществляется поиск по данному пользователем ранее построенному индексу.
- Все найденные документы ранжируются согласно BM25.
- В качестве результата выводит список файлов и номера строк в каждом файле, на которых встречается каждое слово из запроса.
- Синтаксис запросов позволяет использовать ключевые слова "AND" и "OR".
- При некорректном запросе пользователю выводится соответствующая ошибка.
- Поиск регистронезависимый

### Синтаксис запросов

Должен поддерживать скобки а также операции AND и OR (регистр имеет значение).
Таким образом, в качестве запроса выступают логические выражения. Разделитель между словами - пробел(ы)

Следующие запросы считаются корректными:
 - "for"
 - "vector OR list"
 - "vector AND list"
 - "(for)"
 - "(vector OR list)"
 - "(vector AND list)"
 - "(while OR for) and vector"
 - "for AND and"

Некорректными запросами считаются:
 - "for AND"
 - "vector list"
 - "for AND OR list"
 - "vector Or list"
