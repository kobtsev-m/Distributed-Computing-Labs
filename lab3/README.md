## Лабораторная №3: Введение в технологию MapReduce

### Цель работы:

Ознакомиться с технологией MapReduce, программным обеспечением Hadoop для организации вычислений по технологии MapReduce, получить опыт решения практических задач.

### Ход выполнения:

**Установка и настройка Hadoop и MapReduce выполнялась по туториалу по ссылке: https://medium.com/@jootorres_11979/how-to-set-up-a-hadoop-3-2-1-multi-node-cluster-on-ubuntu-18-04-2-nodes-567ca44a3b12**

1) Была создана 1 виртуальная машина Ubuntu 22.04 (4GB RAM, 32GB storage, 4 vCPU)

<img width="403" alt="image" src="https://user-images.githubusercontent.com/44488666/229760472-96860aef-b747-4518-befc-7012910456f5.png">

2) Далее было настроено подключение по ssh

3) Установлена java версии 8

4) Скачен и настроен hadoop версии 3.3.5

5) Создан пользователь для работы с hadoop и правами sudo - hadoopuser

6) Исходная виртуальная машина была склонирована 2 раза так, что всего получилось 3 VM; Для каждой был сгенерирован уникальный MAC и IP адреса в сети

<img width="409" alt="image" src="https://user-images.githubusercontent.com/44488666/229761851-fdd007a2-0367-452c-aba9-4f205fe7cc8e.png">

<img width="645" alt="Снимок экрана 2023-04-04 в 17 17 56" src="https://user-images.githubusercontent.com/44488666/229762130-eb91e5b2-b74a-49cd-b1ff-47c39f8c00a1.png">

7) На каждой машине были прописаны хосты сети и собственный hostname

`/etc/hosts`:

<img width="567" alt="image" src="https://user-images.githubusercontent.com/44488666/229762806-0865849b-da46-4414-a383-5e1b59bb03d4.png">

`/etc/hostname` (пример для master):

<img width="143" alt="image" src="https://user-images.githubusercontent.com/44488666/229763229-feccb2aa-7611-4b80-a47b-48e38e04af99.png">

8) Донастроен hadoop и прописаны worker'ы (в нашем случае hadoop-node1 и hadoop-node2)

9) Скопированы настроки hadoop и ssh-key с master VM на worker'ы

10) Была вмонтирована и запущена файловая система hadoop - HDFS, после чего были успешно созданы DataNodes на worker'ах

Результат виден в интерфейсе hadoop на скринах:

![1) hadoop started overview](https://user-images.githubusercontent.com/44488666/229764617-11690ea3-e66b-4a39-9493-bf20d6c76341.png)

![2) hadoop started datanodes](https://user-images.githubusercontent.com/44488666/229764483-0b335961-c417-473c-aa4c-a0f7974cafb2.png)

11) Длаее была запущена система усправления кластером hadoop - YARN

![3) hadoop cluster](https://user-images.githubusercontent.com/44488666/229765537-7cca07be-0683-4fd5-9f3e-a71e7c927e3b.png)

**На этом этап настройки закончился, после идёт запуск тестовой программы для mapreduce:**

Был запущен один из примеров mapreduce, предоставляемый hadoop в папке shared, он отработал успешно:

![4) running mapreduce example](https://user-images.githubusercontent.com/44488666/229766079-84cf5e28-b72c-4b84-a305-569a520fd61d.png)

![5) running mapreduce exmaple result](https://user-images.githubusercontent.com/44488666/229766086-af178a1d-1ac5-4983-8a81-b3a9f548276e.png)

**Теперь запуск приложения wordcount:**

1) Создан файл WordCount.java

![6) creating wordcount file](https://user-images.githubusercontent.com/44488666/229767069-104bfdb4-2122-4ff9-95d7-d8dec5a64ca4.png)

2) WordCount.java был скомпилирован

![7) compiling wordcount file](https://user-images.githubusercontent.com/44488666/229767074-79cd3de1-e413-44b3-b024-764ce96ff386.png)

3) Созданы файлы в системе HDFS для подачи на вход (input) для программы worcount

![8) creating inputs in HDFS for wordcount](https://user-images.githubusercontent.com/44488666/229767076-16308306-03d4-42d4-bf87-4ce6caa83f42.png)

4) Была запущена программа wordcount

![9) running wordcount](https://user-images.githubusercontent.com/44488666/229767078-d96f3175-5cf9-4197-9cf3-54d7d0128163.png)

5) Просмотр резльтата выполнения wordcount

![10) wordcount result](https://user-images.githubusercontent.com/44488666/229767080-f27f5df0-5082-430e-9e20-08c822b2ba8c.png)
