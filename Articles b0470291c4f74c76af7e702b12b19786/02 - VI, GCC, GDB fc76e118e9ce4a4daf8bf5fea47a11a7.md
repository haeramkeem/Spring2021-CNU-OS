# 02 - VI, GCC, GDB

## gcc 옵션 참고해라

![02%20-%20VI,%20GCC,%20GDB%20fc76e118e9ce4a4daf8bf5fea47a11a7/image1.png](02%20-%20VI,%20GCC,%20GDB%20fc76e118e9ce4a4daf8bf5fea47a11a7/image1.png)

- 딴건몰라도 -o, -c, -g는 알아야된다

## make

### 기본문법

Main.c, read.c, write.c, io.h를 가지고 test를 만드는 make코드

![02%20-%20VI,%20GCC,%20GDB%20fc76e118e9ce4a4daf8bf5fea47a11a7/image2.png](02%20-%20VI,%20GCC,%20GDB%20fc76e118e9ce4a4daf8bf5fea47a11a7/image2.png)

destination : dependency

command

이런식으로 쓰면 된다

### make 매크로 예시

대문자들이 메크로니까 잘 읽어봐라

![02%20-%20VI,%20GCC,%20GDB%20fc76e118e9ce4a4daf8bf5fea47a11a7/image3.png](02%20-%20VI,%20GCC,%20GDB%20fc76e118e9ce4a4daf8bf5fea47a11a7/image3.png)

- **OBJECT** 변수 : 만들 오브젝트 파일들을 명시
- **CC** 변수 : gcc
- **CFLAGS** 변수 : 컴파일 옵션

### make 자동 메크로

![02%20-%20VI,%20GCC,%20GDB%20fc76e118e9ce4a4daf8bf5fea47a11a7/image4.png](02%20-%20VI,%20GCC,%20GDB%20fc76e118e9ce4a4daf8bf5fea47a11a7/image4.png)

- **$^** : 모든 dependency를 가져오는 메크로

### make suffixes

![02%20-%20VI,%20GCC,%20GDB%20fc76e118e9ce4a4daf8bf5fea47a11a7/image5.png](02%20-%20VI,%20GCC,%20GDB%20fc76e118e9ce4a4daf8bf5fea47a11a7/image5.png)

- **.SUFFIXES** : 확장자 목록?이라고 생각하면 된다
    - 사용할때 **%.확장자**이래해주면 그 확장자를 가진 모든 파일을 대변하는 느낌

## GDB 사용하기

### 실행

- **컴파일시 -g 옵션 반드시 넣어야 된다**
- 이렇게 gdb 뒤에 실행파일 이름을 인자로 주면 된다

gdb ./a.out

### 명령어

- **l** : 소스코드를 1-10줄까지 보여준다
- **b 5** : 5번째 줄에 bp를 건다
- **r** : 소스코드를 처음부터 실행시킨다
- **c** : 소스코드를 멈춘부분부터 실행시킨다
- **s** : 소스코드를 한줄한줄 실행하되 함수가 나오면 함수 내부로 들어간다
- **n** : 소스코드를 한줄한줄 실행시키되 함수가 나와도 함수 내부로 들어가지 않는다
- step이 드가는거고 next가 안드가는거 기억해라

## 기타

- grep : 정규식마냥 검거하는 것
- diff : 두 파일의 차이를 보여주는 것

## 환경변수

- 쉘에 명령어를 입력하게 되면 그 명령어를 파싱해 어떤 작업을 요청했는지를 알아내게 되고, 그 작업을 처리할 프로그램을 시스템 안에서 찾게 된다
- 이때 프로그램을 찾을때 뒤지는 디렉토리들을 저장한 곳이 **환경변수(PATH)**이다
- **export $PATH**를 통해 지금 저장된 환경변수들을 열람할 수 있고
- **export PATH = $PATH:주소**를 통해 현 환경변수의 뒷쪽에 환경변수를 추가할 수 있다
    - 환경변수에 들어가는 값은 여러갠데 이들을 **:**을 통해 구분짓게 된다 - $PATH:주소 이렇게 해주는 것은 현재의 환경변수를 $PATH로 불러오고 :주소 이렇게 해줌으로써 현재의 환경변수의 뒤에 새로운 주소를 추가하는 것
    - 다만 export를 이용한 환경변수 추가는 현재의 터미널에서만 유효함. 현재의 터미널을 종료하거나 터미널을 하나 더 실행시킬 경우에는 적용되지 않는 일시적인 변경이다.

## 로그

- 리눅스는 현재의 상황을 로그로 남겨서 저장한다
- **syslog** : 시스템과 커널의 로그
- **dmesg** : 부팅 메세지?