ПЕРЕД УСТАНОВКОЙ
Зайти в  Rasperry Pi Configuration->Interfaces и включить интерфейсы I2C и Serial

			
УСТАНОВКА
1. Установить пакет  SphinxDetectors-1.0.X-Linux.deb одним из следующих способов:
	a) двоиным щелчком на самом установочном пакете 
	б) в терминале переити в папку с установочным пакетом и выполнить команду 
		sudo dpkg -i SphinxDetectors-1.0.X-Linux.deb
		Этот способ позволяет посмотреть ошибки во время установки
		

2. Запустить  keygen для генерации ключа. Для этого в терминале перейти в папку с установленным пакетом и выполнить комманды:
	sudo chmod +x keygen
	sudo ./keygen
	
	Ключ server.key должен сгенерироваться в папке /var/lib/SphinxDetectors/

4. Выполнить  "sudo /etc/init.d/SphinxDetectors_d start" либо перезагрузить систему.
При включении Rasberry программа будет автоматически запущена в фоновом режиме.

5. Вполнить sudo /etc/init.d/SphinxDetectors_d status и убедиться что сервер запущен.


УДАЛЕНИЕ
Для удаления пакета в терминале ввести команду:
	sudo apt-get remove sphinxdetectors
Перед установкой новой версии старую нужно удалить. При удалении ключ server.key не удаляется, 
и будет использоваться  при установке новой версии. 
	

ПРОВЕРКА

Устанавливаем клиент ONVIF Device Manager (https://netix.dl.sourceforge.net/project/onvifdm/onvifdm-v2.2.250/odm-v2.2.250.msi)

логин - admin
пароль по умолчанию - admin 

Выполнить Device list -> Add  (поправить IP, порт указать 8080)
http://192.168.29.129:8080/onvif/device_service

Сообщения должны появляться на вкладке Events


ЛОГИРОВАНИЕ
Логи пишутся по пути   /var/logs/SphinxDetectors.error
