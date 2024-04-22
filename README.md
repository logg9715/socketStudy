# socketStudy
소켓프로그래밍 연습

# 클라우드 중간 정리

1. 아파치, 톰캣 등이 잘 돌아지는지 확인하는 문제 3가지 
- systemctl status 확인
- 해당 포트 열려있는지 확인 (netstat)
- http 클라이언트(크롬 등)로 직접 서버에 들어가보면 된다. (localhost:8080)

<br>

2. 3티어 아키텍쳐 3가지 구성요소 쓰고, 구조 그림으로 그리고, 장단점 서술 
- 구성요소 : 프리젠테이션 계층, 어플리케이션 계층, 데이터베이스 계층
- 장점 : 각 계층이 독립적으로 개발되므로 유지보수가 쉽고 재사용이 용이하다.
- 단점 : 구조가 복잡해지고 부하가 증가함
- 구조도 : 프리젠테이션 <-- (구성요소 요청, 응답) --> 어플리케이션 <-- (DB정보 요청, 응답) --> DB

<br>

3. 웹 서버의 역할과 장점  // 웹서버는 아파치, was는 톰캣
- 역할 : 정적인 콘텐츠 처리, 방화벽 밖에 있어서 보안 유지, 로드밸런싱
- 장점 : 정적인 콘텐츠 처리가 빠름, was url을 숨기고 포트번호를 숨겨서 보안 강화, 다수의 was에 업무 분배

<br>

4. 아파치&톰캣 상황 부여, 올바르게 돌아가도록 설정파일 수정(was여러개, 각각 다른 app이 있는 경우)
- /etc/libapache2-mod-jk/workers-properties 수정
```
#/etc/libapache2-mod-jk/workers-properties

workers.tomcat_home=/var/lib/tomcat9
workers.java_home=/usr/lib/jvm/default-java
worker.list=was1,was2,was3

worker.was1.port=port1(해당 port)
worker.was1.host=ip1(해당 ip)
worker.was1.type=ajp13 

worker.was2.port=port2(해당 port)
worker.was2.host=ip2 (해당 ip)
worker.was2.type=ajp13

# 반복...
```
- /etc/apache2/sites-available/000-default.conf 수정
```
#/etc/apache2/sites-available/000-default.conf

<VirtualHost *:80>
  ServerName 127.0.0.1
  ServerAdmin webmaster@localhost
  DocumentRoot /var/www/html 
  JkMount /app1/* was1
  JkMount /app2/* was2
  JkMount /app3/* was3
  # 이 경우에는 url에 /app1/..이 오면 이 이후로는 무조건 was1의 app을 찾아간다.
</VirtualHost>
```

<br>

5. 로드벨런서: 라운드로빈 웨이티드 어쩌구 작동방식 설명
- 로드밸런싱 : 웹서버(아파치)가 트래픽을 분산하는 기술
  - scale up : 디스크 할당량 키워서 대역폭 증가
  - scale out : 부하를 처리할 서버 개수 증가
- Round Robin : 순차적으로 일 할 서버를 선택함, 돌아가면서 순서대로 서버 할당해줌
- weighted Round Robin : 업무 비중에 따라 서버를 선택해줌, 비중에 따라 변수를 저장하고 변수가 작은 서버에게 일을 골구로 할당

<br>

6. ~ 리눅스 관련 문제

<br>

9. 아파치 1개에 톰캣 여러개 물린 상황에서 작동하는 url ox문제
- 3주차 34페이지 실습6 참조

<br>


## 리눅스 관련 문제 예측

- 텔넷 접속 : telnet 192.168.0.10
- ssh 접속 : ssh 아이디@ip주소
- scp : scp -p 포트번호 /보낼파일위치(서버) /받을파일위치(로컬), scp -p 포트번호 -r /보낼파일위치(로컬) /받을파일위치(서버), 포트 생략하면 22임
- head, tail : 파일의 앞뒤만 보여줌, head -3하면 3줄만 표시됨
- more : 내용을 page단위로 보여줌
- less : more의 진화형
- find : 파일찾기, find /주소 (-name, -user, -perm(파일권한), -size) (옵션에 대한 내용)
- 0=표준입력, 1=표준출력, 2=표준에러

<br>

- vi 팁 : 그냥 vi 치고, 저장할때 :wq 파일명 치면 파일명으로 저장됨
  - 명령모드(초기화면) -> 입력모드(a나 i입력)

<br>

- 리눅스 사용자 보기 : cat /etc/passwd
- 사용자 추가 : adduser
- 비밀번호 변경 : passwd 유저명
- 그룹 관련
  - groupadd 그룹명 : 그룹생성
  - groupmod --new-name 이름 이름 : 이름변경
  - groupdel 이릅 : 그룹삭제
  - gpasswd 옵션(-a 멤버추가, -A 어드민, -d 멤버삭제) : 그룹 관리, 비번설정

<br>
 
- 로그인 : su 계정이름
- 소유자변경 : chown 계정명 파일이름
- 하드링크 : ln 파일명 링크파일명, ln -s 파일명 링크파일명 (심볼릭 링크)
- 프로세스 관계도 표시 : pstree
- 백그라운드 실행 : 명령 &
