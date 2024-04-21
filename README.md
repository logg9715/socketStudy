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
- 구조도 : 프리젠테이션 <-- (구성요소 요청, 응답) --> 어플리케이션 <-- (DB정보 요청, 응답) --> DB

<br>

3. 웹 서버의 역할과 장점  // 웹서버는 아파치, was는 톰캣
- 역할 : 정적인 콘텐츠 처리, 방화벽 밖에 있어서 보안 유지, 로드밸런싱
- 장점 : 정적인 콘텐츠 처리가 빠름, was url을 숨기고 포트번호를 숨겨서 보안 강화, 다수의 was에 업무 분배

<br>

4. 아파치&톰캣 상황 부여, 올바르게 돌아가도록 설정파일 수정
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
```

<br>

5. 로드벨런서: 라운드로빈 웨이티드 어쩌구 작동방식 설명

<br>

6. 7. 리눅스 관련 문제

<br>

9. 아파치 1개에 톰캣 여러개 물린 상황에서 작동하는 url ox문제
- 3주차 34페이지 실습6 참조

<br>

