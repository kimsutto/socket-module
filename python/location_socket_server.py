import socket
import pymysql
import base64
from _thread import *
from sel_add_k import add_kakao




HOST = '0.0.0.0'
PORT = 8888

place_name = ''
place_address = ''
category_str = ''
category_int = 0
etc_info = ''
platform = 0
entrance_b = 2
seat_b = 2
parking_b = 2
restroom_b = 2
elevator_b = 2
phone_num = '' 
img_num = 'default'
locLat = 0.0
locLon = 0.0
userId = 0
place_type = 0

#받은 문자열 파싱해서 DB에 추가 

def db_insert(_data):

    con = pymysql.connect(host='127.0.0.1',user='root',password='',db='',port=3306, charset='utf8')
    cur = con.cursor()

    str_data = _data
    loc_db =str_data.split(',')
    
    place_name = loc_db[0]
    place_address = loc_db[1]
    category_str = loc_db[2]
    if category_str =='장애인용 공용 화장실':
        category_int = 1
    elif category_str =='숙박업소':
        category_int = 2
    elif category_str == '보건소':
        category_int = 3
    elif category_str == '음식점':
        category_int = 4
    elif category_str == '주차장':
        category_int = 5
    elif category_str == '문화센터':
        category_int = 6
    elif category_str == '교육센터':
        category_int = 7
    else : #병원
        category_int = 8

    if loc_db[3] =='':
        etc_info = '-'
    else :
        etc_info = loc_db[3]

    if loc_db[4]=='true' and loc_db[5]=='true':
        platform = 1
    elif loc_db[4]=='true':
        platform = 2
    else :
        platform = 3

    if loc_db[6] == 'true' :
        entrance_b = 1
    else :
        entrance_b = 0
    if loc_db[7] == 'true' :
        seat_b = 1
    else :
        seat_b = 0
    if loc_db[8] == 'true' :
        parking_b = 1
    else :
        parking_b = 0
    if loc_db[9] == 'true' :
        restroom_b = 1
    else :
        restroom_b = 0
    if loc_db[10] == 'true' :
        elevator_b = 1
    else :
        elevator_b = 0

    
    locLat = float(loc_db[14])
    locLon = float(loc_db[15])

    phone_num = loc_db[11]
    user_id = loc_db[12]
    place_type = loc_db[13]   
 
    add_sel()

    #location insert DB query
    sqldata = (user_id,place_type,place_name,place_address,platform,locLat,locLon,category_int,entrance_b,seat_b,parking_b,restroom_b,elevator_b,phone_num,etc_info)
    sql3 = 'insert into location(userNo,locType,locDate,locName,locAddress,locPlatform,locLatitude,locLongitude,locCategory,locEnter,locSeat,locPark,locToilet,locElevator,locImg,phoneNum,userAdditional) values(%s ,%s,now(),%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,default,%s,%s);'
    cur.execute(sql3,sqldata)
    con.commit()
    con.close()

    #selenium 보내는 code
    if place_type == 1 :
        if platform == 1 :
            add_kakao(place_name,place_address,category_str,phone_num,etc_info,entrance_b,seat_b,parking_b,restroom_b,elevator_b)
            sel_add_g.add_google(place_name,category_str,place_address)
        elif platform == 2 :
            sel_add_k.add_kakao(place_name,place_address,category_str,phone_num,etc_info,entrance_b,seat_b,parking_b,restroom_b,elevator_b)
        else :
            sel_add_g.add_google(place_name,category_str,place_address)
      else :
        if platform == 1 or platform ==2 :
            sel_mod_k.mod_kakao(place_address,etc_info,entrance_b,seat_b,parking_b,restroom_b,elevator_b)   




def add_sel():
	if place_type == 1 :
		if platform == 1 :
			add_kakao(place_name,place_address,category_str,phone_num,etc_info,entrance_b,seat_b,parking_b,restroom_b,elevator_b)

#입력 데이터 받는 부분

def threaded(client_socket, addr):

    print('Connected by :', addr[0])

    while True:

        try:
            data = client_socket.recv(9000)

            if not data:
                print('Disconnected by ' + addr[0])
                break

            print('' + data.decode('utf-8')) 
            str_data = data.decode('utf-8')
            db_insert(str_data)


        except ConnectionResetError as e:
            print('Disconnected by ' + addr[0],':',addr[1])
            break

    client_socket.close()



#서버 소켓 생성


server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((HOST, PORT))
server_socket.listen()

print('server start')


while True:

    print('wait')


    client_socket, addr = server_socket.accept()
    start_new_thread(threaded, (client_socket, addr))

server_socket.close()

