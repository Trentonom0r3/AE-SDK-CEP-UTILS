
# HellowWorld "ExternalObject"

[CEP & C++ネイティブコードによるPhotoShop拡張](https://qiita.com/MAA_/items/b1a35ab73af9f7b327e0) <br>
にあったExternalObjectサンプルをVisual studio 2022でビルド出来るようにしました。

* ./include<br>必要なAdobe Extend Tool kitにあったヘッダ
* ./jsx<br>サンプルのjsxファイル
* HellowWorld.cpp<br>サンプルの奴。機能のほとんどが入ってます。
* 後はVisualStudio2022のファイルです

以上の者だけでビルドが出来ます。<br>
詳細は上記の記事を読んでください。
<br>

ビルドすると<x64>のフォルダの中に<Release>/<Debug>フォルダの中のdllフォルダの中にDLLが出力されます。<br>
<br>
とりあえず、そのdllとHellowWorld.jsxをScriptsフォルダに入れてjsxを実行して確認ができます。<br>


## ExternalObject

<b>ExternalObject</>とはぶっちゃけて言えばC++で作るプラグインのシステムで、After Effectsスクリプトの機能拡張を行う者です。<br>

Adobe ExtendScript Toolkit CCがインストールされていれば、以下の場所にある<b>JavaScript Tools Guide CC.pdf</b>に詳細な事が説明されてますが、[上記](https://qiita.com/MAA_/items/b1a35ab73af9f7b327e0) の記事に簡単に説明されていたので、ちょっと試して見ました。

```
C:\Program Files (x86)\Adobe\Adobe ExtendScript Toolkit CC\SDK
```

とりあえず、HellowWOrldをビルド出来るようにしてみました。<br>


## License

This software is released under the MIT License, see LICENSE<br>

## Authors

bry-ful(Hiroshi Furuhashi)<br>

twitter:[bryful] (https://twitter.com/bryful) <br>

bryful@gmail.com<br>

# References

[CEP & C++ネイティブコードによるPhotoShop拡張 https://qiita.com/MAA_/items/b1a35ab73af9f7b327e0](https://qiita.com/MAA_/items/b1a35ab73af9f7b327e0) <br>


