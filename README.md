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



# 클라우드 기말 정리

### 1. 온프레미스 환경(기업이 자체적으로 IT 인프라를 소유, 관리 및 운영하는 경우) DB센터 정의, 안정적인 운용을 위해 필요한 항목 3가지 이상 작성
- 전력 공급
- 인터넷 환경
- 항온항습
- 보안 

### 2. 클라우드 운영환경의 장점을 서술하시오. (온프레미스에 비해)
- 공간 비용 절약 가능
- 인건비 절약 가능 (서버 운영 인원 감소)
- 랙장비 설치 비용 절약 가능
- 여러 장애 상황 대비 가능(물리적인 재난-화재 정전 등, 트래픽 과부하) 

### 3. 클라우드 큰 종류 3가지 IAAS PAAS SASS 약어 설명, 정의 설명
- IAAS : infrastructure as a service < 네트워킹, 컴퓨터 (가상 & 하드웨어), 데이터 저장공간 제공
- PAAS : platform as a service < + 운영체제, 미들웨어, 런타임, DB 제공
- SAAS : software as a service < + 소프트웨어 제공


### 4. 로드벨런서, 스케일 업,(스케일 다운) 설명, 로드밸런서의 의미 설명
- 로드밸런서 :  (ELB = > elastic load Balancer 아마존용 로드밸런싱) 여러대의 서버에서 동일한 서비스를 제공하게 해줌, 규칙에 따라 부하를 분산시키는 네트워크 장치 또는 프로세스
- 스케일 업 : 서버를 더 좋은걸로 업그레이드 > 성능 증가, 네트워크 대역폭 증가
- 스케일 다운 : 기존 서버 자원 줄이기(cpu할당량 축소 등) > 실행중인 어플리케이션을 종료하지 않아도 됨.
- 스케일 아웃 : 부하를 처리할 서버 증설, 오토스케일링 사용 가능 > 유연하게 서버 확장-축소가 가능, 하나의 서버가 오작동해도 다른 서버가 서비스 제공 가능
- 스케일 인 : 스케일 아웃을 줄이는 버젼
<br>

### 5. SPED의 로드밸런싱 5가지 방법 서술
- 라운드로빈 Round Robin : 순서대로 서버를 선택
- 가중 라운드로빈 Weighted Round Robin : 서버마다 가중치를 매기고, 가중치가 가장 적은 서버를 연결
- 해쉬 hash (Sticky Session) : 로드밸런서에 의해 클라이언트가 연결된 한 서버에만 계속 연결되는 방법
- Least Connection : 연결수가 가장 적은 서버에 연결
- 응답시간 Response Time : 응답속도가 가장 빠른 서버 연결

### 6. AWS RDS서버 고가용성을 확보하는 방법 설명, 장애 발생시 해결하는 방법 서술
- 고가용성 확보하는 방법 : 다른 가용 영역에 RDS 인스턴스를 추가로 확보하여 원래 RDS인스턴스에 문제 생기면 대체하여 사용
- 장애 해결방법 : 사용중인 RDS에 문제가 생기면 스탠바이 인스턴스로 대체한다. 

### 7. 오토 스케일링 문제 

-- 오토스케일링 정의 설명 (스케일 인 , 스케일 아웃 활용하여 설명)

``` text
오토스케일링은 설정된 정책에 따라 실시간으로 시스템의 부하를 모니터링하고,
트래픽 증가나 감소에 대응하여 자동으로 서버 인스턴스 수를 조정하는 기술 
(부하가 늘어나면 스케일 아웃으로 추가 서버 인스턴스를 생성, 어플리케이션 수요가 감소하면 불필요한 서버 인스턴스를 종료한다.)
```


-- aws의 오토스케일링의 구성요소 그림과 함께 설명, 각각 요소 설명 (AWS_6의 11페이지 자동 확장 실습 페이지 참조)
![image](https://github.com/logg9715/socketStudy/assets/127168700/c83f1b0b-ccc8-41cd-933c-eb898df34e97)

- ASG 오토스케일링 그룹 : EC2 인스턴스 집합의 논리 그룹, 인스턴스 모니터링, 인스턴스 최대-최소값 설정& 스케일 인-아웃 제어
- startup template 시작 템플릿 : 새 EC2를 시작할때 쓰는 템플릿
- load Balancer 로드밸런서 : 여러 인스턴스에 트래픽을 분산시킨다. 
- target group 대상 그룹 : 로드밸런서가 연결시켜줄 인스턴스들을 정의한 그룹, 오토스케일링에 따라 추가되고 삭제될 수 있음. 


-- 오토스케일링 인스턴스 옵션 4가지 설명, 그 중 온디멘트 방식 있는 것을 예제와 함께 설명 (바로 전꺼 학습지 10페이지 참조) -> cpu사용률을 예로 들어서 설명
인스턴스 개수 조정 옵션 : 
1. 항상 지정된 수의 인스턴수 유지 = 자동 확장 그룹에서 지정한 인스턴스 개수를 유지
2. 수동 조정 = 사용자가 직접 조정
3. 일정 기반(기간?) 조정 = 특정 시간대에 따라 인스턴스를 확장-축소함
4. 온디맨드 기반 조정 = EC2지표 (Metric)기반으로 조정 , 예시) 평균 cpu 사용율이 8할 이상이면 인스턴스 늘림





