import 'dart:io';
import 'dio_new.dart';

HttpResponse handleResponse(Response? response,
    {HttpTransformer? httpTransformer}) {
  httpTransformer ??= DefaultHttpTransformer.getInstance();

  // 返回值异常
  if (response == null) {
    return HttpResponse.failureFromError();
  }

  // token失效
  if (_isTokenTimeout(response.statusCode)) {
    return HttpResponse.failureFromError(UnauthorisedException(
        message: "没有权限", status: response.statusCode.toString()));
  }
  // 接口调用成功
  if (_isRequestSuccess(response.statusCode)) {
    return httpTransformer.parse(response);
  } else {
    // 接口调用失败
    return HttpResponse.failure(
        errorMsg: response.statusMessage,
        errorCode: response.statusCode.toString());
  }
}

HttpResponse handleException(Exception exception) {
  var parseException = _parseException(exception);
  return HttpResponse.failureFromError(parseException);
}

/// 鉴权失败
bool _isTokenTimeout(int? code) {
  return code == 401;
}

/// 请求成功
bool _isRequestSuccess(int? statusCode) {
  return (statusCode != null && statusCode >= 200 && statusCode < 300);
}

HttpException _parseException(Exception error) {
  if (error is DioError) {
    int? errCode = error.response?.statusCode;
    switch (error.type) {
      case DioErrorType.connectTimeout:
      case DioErrorType.receiveTimeout:
      case DioErrorType.sendTimeout:
        return NetworkException(message: "请求超时");
      case DioErrorType.cancel:
        return CancelException(error.error.msg);
      case DioErrorType.response:
        try {
          switch (errCode) {
            case 400:
              return BadRequestException(
                  message: "请求语法错误", status: errCode.toString());
            case 401:
              return UnauthorisedException(
                  message: "没有权限", status: errCode.toString());
            case 403:
              return BadRequestException(
                  message: "服务器拒绝执行", status: errCode.toString());
            case 404:
              return BadRequestException(
                  message: "无法连接服务器", status: errCode.toString());
            case 405:
              return BadRequestException(
                  message: "请求方法被禁止", status: errCode.toString());
            case 500:
              return BadServiceException(
                  message: "服务器内部错误", status: errCode.toString());
            case 502:
              return BadServiceException(
                  message: "无效的请求", status: errCode.toString());
            case 503:
              return BadServiceException(
                  message: "服务器挂了", status: errCode.toString());
            case 505:
              return UnauthorisedException(
                  message: "不支持HTTP协议请求", status: errCode.toString());
            default:
              return UnknownException(error.error.msg, errCode.toString());
          }
        } on Exception catch (_) {
          return UnknownException(error.error.msg, error.toString());
        }

      case DioErrorType.other:
        if (error.error is SocketException) {
          return NetworkException(message: error.message);
        } else {
          return UnknownException(error.message, errCode.toString());
        }
      default:
        return UnknownException(error.message, errCode.toString());
    }
  } else {
    return UnknownException((error as HttpException).message, (error).status);
  }
}
