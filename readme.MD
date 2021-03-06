# 电子教室系统需求说明 v2（2017-11-12）

**一、** **主要功能**

1、设备管理功能

2、课程服务功能

**二、** **机房使用目的**

1. 上课：使用机房的计算机进行实验课程的授课，完成相关实验。

2. 自由上机（预约上机）：学生自由使用计算机完成各自的目的。

3. 机器维护：用于教师机的

**三、** **教师端程序功能**

1. 教师机开机后自动启动教师端程序，程序界面要求选择机房用途：“上课”、“自由上机”、“维护”。

l 上课：使用机房进行实验课程的教学，需要输入教师、班级、课程信息，可以实现考勤功能。

l 自由上机：使用机房进行自主用机，如自由上机、教学竞赛等教学活动，需要输入教师、教学活动名称等信息，可以实现考勤功能。

l 维护：实验室对教师机进行软硬件安装管理。

2. 如果选择“机器维护”，则输入密码后正常使用机器；

3. 如果选择“上课”、“自由上机”，则完成后面的功能。

4. 登记本次实验课的课程信息

具体登记项目参见实验室工作日志：日期、周次、星期、节次、课程名称、授课专业及班级、实验学时（默认为 2）、指导教师姓名、应到人数、实到人数、每组人数（默认为 1）、实验类别、实验要求、实验类型等。（动态调整课程信息，滞后设计）

如果不是第一次使用该机房也不是第一次上该课程，则在输入教师名称、课程名称、班级信息后，其他信息自动填充应到人数、每组人数、实验类别、实验要求、实验类型。（建议：建立一个本机房上课的信息表，登记教师所授课程的信息，自动导入上一次的课程信息）。

允许教师对该课程的信息进行修改，包括教师姓名、班级、课程名称、人数等。（建议在考勤界面进行此项修改）。

5. 考勤功能

考勤是指教师本次实验课程需要点名考勤。

考勤操作：要求学生输入考勤信息：学号和姓名。

(1) 如果步骤 4 选择“上课”，必须考勤（因为实验室需要统计计算机的使用情况）。自动导入学生名单。如果步骤 4 选择“自由上机”，则由教师自行决定是否考勤，如果需要考勤则自行导入学生名单。

(2) 考勤发起的时间

如果“上课”，教师端启动并完成本次授课的信息输入后，在半小时内（系统可更改默认的发起考勤时间间隔）教师随时可以发起考勤。如果半小时内教师没有主动发起考勤，则系统出现界面提示教师考勤，教师确认考勤后由系统启动考勤操作；如果 15 分钟内教师没有在提示界面上确认考勤，则系统自动发起考勤操作。考勤数据存入服务器的系统数据库。

如果“自由上机”，询问教师是否考勤。若要考勤，半小时内需导入学生名单，并人工发起考勤操作，考勤数据存入服务器上的系统数据库。若半小时内教师没有发起考勤操作，则系统自动发起考勤操作，从发起考勤操作开始，15 分钟后，将考勤名单存到指定的 ftp 服务器。

(3) 考勤次数

允许考勤多次。本节次课第一次考勤完成后（不管是教师人工发起还是系统自动发起），如果教师再次发起考勤操作，询问“是否再考勤一次？”若是，则发起考勤，并将新的考勤数据覆盖前一次的考勤数据。新增加的学生以“迟到”记录，消失的学生以“早退”记录。

(4) 关于考勤名单

考勤名单由授课教师提供，系统提供模版下载。建议：系统内的学生名单文件（本系统也作为考勤记录表）命名为“年月日（8 位）+教师+班级+课程”，内含学号、姓名、专业、班级。

如果该教师是第一次使用本机房上该门课程，由系统提供该班级学生名单（教师可以修改），则必须先导入学生名单。

如果该教师不是第一次使用本机房上该门课程，则系统根据教师名称、课程名称、班级名称自动导入上一次的学生名单（教师可以再修改或多次修改）。

(5) 在教师端可以浏览本次考勤信息，也可界面增加考勤信息记录（针对迟到、请假的学生）。

(6) 也可查看历次考勤信息也可在教师端界面增加考勤信息记录（针对迟到、请假的学生）。某老师给某班某实验课程的考勤信息保存在一个数据表，某次实验课若存在多次考勤，则以该天日期后添加考勤序号作为新的字段（例如，2017 年 11 月 3 日 1、2 节第一次考勤字段为“201711031”，第二次考勤新增加的字段名为“201711032”）。本次考勤信息可由教师本人发系统设定的教师邮箱或老师指定的文件夹，同时存放在服务器上。服务器上的文件夹及本机文件夹可由系统管理员指定及修改。

6. 分发文件功能

从教师端向各学生机指定的文件夹发送文件。

7. 学生机在线监控功能

按机房计算机的实际布局，在教师端程序界面呈现学生机位置，并能动态显示学生机的在线状态。在线计算机还需在该机图标下方显示该机的机位号（或计算机名称）、学生姓名。

8. 屏幕广播功能

可以将教师端屏幕、指定的学生端屏幕广播给所有在线学生端，同时锁死学生端的鼠标与键盘。

**四、** **学生端程序功能**

1. 学生机一开机，启动客户端，寻找教师机，如果教师机没启动，学生机没法退出登录界面；

2.

3) 学生端计算机启动后，自动出现学生端界面，要求学生选择“有老师上课”还是“无老师上课”。此界面一直处于屏幕最前端，直到学生做出选择为止。

4) 如果是“无老师上课”，教师端得先选择“自由上机”。学生端必须输入学生学号和姓名（系统不限制学号及姓名的格式：外学院学生使用机房的时候），然后返回操作系统。输入完毕，学生信息存储到指定的服务器中指定的文件夹。

5) 如果选择“有老师上课”，则返回操作系统，学生即可自行用机，等待教师端发起考勤；

当教师端发起考勤时，学生端须输入正确的学号和姓名（与教师端数据库进行验证），否则无法退出此考勤界面。输入完毕，存储和发送学生考勤。

如果在规定时间内教师端没有发起考勤，则认为是“自由上机”，发起“自由上机”的考勤处理。

**五、** **统计分析功能**

1. 统计实验室的使用次数

只统计有教师登记“上课”的实验室使用次数：按时间段（精确到日）统计、分各专业与全部专业专业统计。

2. 统计每台计算机的使用次数

**六、** **系统维护功能**

1、 账号管理

只设置一个管理员账号及其密码。

2、 设置本机房名称（教师机的名称会设置为该室的房号，系统维护模块有设置实验室基本信息的功能）

3、 机位号设置

按机房内部计算机的实际放置设置教师端屏幕上的机位布局图，一一对应。也可单独调整个别机位号。

4、 设置发起考勤操作的时间

教师端“5 考勤发起的时间”中的 2 个时间间隔（半小时、15 分钟）；学生端发起考勤的时间（1 小时）。

5、 设置考勤数据存放的服务器路径

6、 设置实验室工作日志的各个项目及某些项目取值范围的数据表

课程名称、班级名称、专业名称、教师名单、实验类别、实验要求、实验类型等。

**七、** **其他功能**

1. Word 文档打印及 excel 文档的输出：

2. 隐藏学生端的客户端进程。

3. 本系统的加密功能

补充：

1、 登录界面

教师登录：姓名

取消注册功能

2、 工作日志界面

指导老师姓名不需要填写

班级放上一行

课程性质（=实验要求：必修、选修、其他）

3、 使用目的

如果是“课程实验”或“竞赛培训”；所有必填；

如果“理论授课”；填：课程名称、班级；

否则是其他选项，填写：教学活动名称，实到人数、使用时长（小时），不填“节次”。

(新建)

4、 班级管理 改成 课堂管理

如果“实验课程”或“技能训练”或“理论授课”，系统自动导入名单；

点名；填写设备故障；提建议。

点名 改成 马上点名；自动点名时间设置（默认 15 分钟）。

考勤记录管理 改成 考勤细节，放第一页，增加机位号；学生信息 改成 考勤统计；第 i 次考勤 改成 日期。

5、 集体提问

6、 Cmd 改成 远程命令；添加常用命令。
