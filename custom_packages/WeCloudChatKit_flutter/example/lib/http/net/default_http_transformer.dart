

import 'package:wecloudchatkit_flutter_example/base/base_response.dart';

import 'dio_new.dart';

class DefaultHttpTransformer extends HttpTransformer {
  @override
  HttpResponse parse(Response response) {
    //TODO 兼容sign
    ResponseData res;

    if(response.data is String){
       res = ResponseData(data:response.data,errorCode: 0);
    }else {
       res = ResponseData.fromJson(response.data);
    }

    if (res.errorCode == 0) {
      return HttpResponse.success(res.data);
    } else {
      return HttpResponse.failure(
          errorMsg: res.errorMsg, errorCode: res.errorCode);
    }
  }

  /// 单例对象
  static DefaultHttpTransformer _instance = DefaultHttpTransformer._internal();

  /// 内部构造方法，可避免外部暴露构造函数，进行实例化
  DefaultHttpTransformer._internal();

  /// 工厂构造方法，这里使用命名构造函数方式进行声明
  factory DefaultHttpTransformer.getInstance() => _instance;
}
