import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_plugin_record_plus/flutter_plugin_record.dart';
import 'package:flutter_plugin_record_plus/utils/common_toast.dart';
import 'package:flutter_plugin_record_plus/widgets/custom_overlay.dart';

typedef startRecord = Future Function();
typedef stopRecord = Future Function();

class RecordWidget extends StatefulWidget {
  final Function? startRecord;
  final Function? stopRecord;
  final double? height;
  final EdgeInsets? margin;
  final Decoration? decoration;

  const RecordWidget(
      {Key? key,
      this.startRecord,
      this.stopRecord,
      this.height,
      this.decoration,
      this.margin})
      : super(key: key);

  @override
  State<RecordWidget> createState() => _RecordWidgetState();
}

class _RecordWidgetState extends State<RecordWidget> {
  // 倒计时总时长
  int _countTotal = 12;
  double starty = 0.0;
  double offset = 0.0;
  bool isUp = false;
  String textShow = "按住说话";
  String toastShow = "手指上滑,取消发送";
  String voiceIco = "images/ic_volume_1.png";

  ///默认隐藏状态
  bool voiceState = true;
  FlutterPluginRecord? recordPlugin;
  Timer? _timer;
  int _count = 0;
  OverlayEntry? overlayEntry;

  @override
  void initState() {
    super.initState();
    recordPlugin = new FlutterPluginRecord();

    _init();

    ///初始化方法的监听
    recordPlugin?.responseFromInit.listen((data) {
      if (data) {
        print("初始化成功");
      } else {
        print("初始化失败");
      }
    });

    /// 开始录制或结束录制的监听
    recordPlugin?.response.listen((data) {
      if (data.msg == "onStop") {
        ///结束录制时会返回录制文件的地址方便上传服务器
        print("onStop  " + data.path!);
        if (widget.stopRecord != null)
          widget.stopRecord!(isUp, data.path, data.audioTimeLength);
      } else if (data.msg == "onStart") {
        print("onStart --");
        if (widget.startRecord != null) widget.startRecord!();
      }
    });

    ///录制过程监听录制的声音的大小 方便做语音动画显示图片的样式
    recordPlugin!.responseFromAmplitude.listen((data) {
      var voiceData = double.parse(data.msg ?? '');
      setState(() {
        if (voiceData > 0 && voiceData < 0.1) {
          voiceIco = "images/ic_volume_2.png";
        } else if (voiceData > 0.2 && voiceData < 0.3) {
          voiceIco = "images/ic_volume_3.png";
        } else if (voiceData > 0.3 && voiceData < 0.4) {
          voiceIco = "images/ic_volume_4.png";
        } else if (voiceData > 0.4 && voiceData < 0.5) {
          voiceIco = "images/ic_volume_5.png";
        } else if (voiceData > 0.5 && voiceData < 0.6) {
          voiceIco = "images/ic_volume_6.png";
        } else if (voiceData > 0.6 && voiceData < 0.7) {
          voiceIco = "images/ic_volume_7.png";
        } else if (voiceData > 0.7 && voiceData < 1) {
          voiceIco = "images/ic_volume_8.png";
        } else {
          voiceIco = "images/ic_volume_1.png";
        }
        if (overlayEntry != null) {
          overlayEntry!.markNeedsBuild();
        }
      });

      print("振幅大小   " + voiceData.toString() + "  " + voiceIco);
    });
  }

  ///显示录音悬浮布局
  buildOverLayView(BuildContext context) {
    if (overlayEntry == null) {
      overlayEntry = new OverlayEntry(builder: (content) {
        return CustomOverlay(
          icon: Column(
            children: <Widget>[
              Container(
                margin: const EdgeInsets.only(top: 10),
                child: _countTotal - _count < 11
                    ? Center(
                        child: Padding(
                          padding: const EdgeInsets.only(bottom: 15.0),
                          child: Text(
                            (_countTotal - _count).toString(),
                            style: TextStyle(
                              fontSize: 70.0,
                              color: Colors.white,
                            ),
                          ),
                        ),
                      )
                    : new Image.asset(
                        voiceIco,
                        width: 100,
                        height: 100,
                      ),
              ),
              Container(
//                      padding: const EdgeInsets.only(right: 20, left: 20, top: 0),
                child: Text(
                  toastShow,
                  style: TextStyle(
                    fontStyle: FontStyle.normal,
                    color: Colors.white,
                    fontSize: 14,
                  ),
                ),
              )
            ],
          ),
        );
      });
      Overlay.of(context)!.insert(overlayEntry!);
    }
  }

  showVoiceView() {
    setState(() {
      textShow = "松开结束";
      voiceState = false;
    });

    ///显示录音悬浮布局
    buildOverLayView(context);

    start();
  }

  hideVoiceView() {
    if (_timer!.isActive) {
      if (_count < 1) {
        CommonToast.showView(
            context: context,
            msg: '说话时间太短',
            icon: Text(
              '!',
              style: TextStyle(fontSize: 80, color: Colors.white),
            ));
        isUp = true;
      }
      _timer?.cancel();
      _count = 0;
    }

    setState(() {
      textShow = "按住说话";
      voiceState = true;
    });

    stop();
    if (overlayEntry != null) {
      overlayEntry?.remove();
      overlayEntry = null;
    }

    if (isUp) {
      print("取消发送");
    } else {
      print("进行发送");
    }
  }

  moveVoiceView() {
    // print(offset - start);
    setState(() {
      isUp = starty - offset > 100 ? true : false;
      if (isUp) {
        textShow = "松开手指,取消发送";
        toastShow = textShow;
      } else {
        textShow = "松开结束";
        toastShow = "手指上滑,取消发送";
      }
    });
  }

  ///初始化语音录制的方法
  void _init() async {
    recordPlugin?.init();
  }

  ///开始语音录制的方法
  void start() async {
    recordPlugin?.start();
  }

  ///停止语音录制的方法
  void stop() {
    recordPlugin?.stop();
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      child: GestureDetector(
        onLongPressStart: (details) {
          starty = details.globalPosition.dy;
          _timer = Timer.periodic(Duration(milliseconds: 1000), (t) {
            _count++;
            print('_count is 👉 $_count');
            if (_count == _countTotal) {
              hideVoiceView();
            }
          });
          showVoiceView();
        },
        onLongPressEnd: (details) {
          hideVoiceView();
        },
        onLongPressMoveUpdate: (details) {
          offset = details.globalPosition.dy;
          moveVoiceView();
        },
        child: Container(
          height: widget.height ?? 40,
          // color: Colors.blue,
          decoration: widget.decoration ??
              BoxDecoration(
                //背景
                color: Colors.white,
                //设置四周圆角 角度
                borderRadius: BorderRadius.all(Radius.circular(4.0)),
                //设置四周边框
                border: new Border.all(width: 1, color: Color(0xffD2D2D2)),
              ),
          margin: widget.margin,
          child: Center(
            child: Text(textShow),
          ),
        ),
      ),
    );
  }

  @override
  void dispose() {
    recordPlugin?.dispose();
    _timer?.cancel();
    super.dispose();
  }
}
