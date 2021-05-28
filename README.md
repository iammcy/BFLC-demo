# BFLC-demo

### 论文来源

标题：《A Blockchain-Based Decentralized Federated Learning Framework with Committee Consensus》

论文链接：https://ieeexplore.ieee.org/document/9293091

### 架构图

<img src=".\imgs\BFLC Architecture.png" alt="BFLC Architecture" style="zoom: 80%;" />

### 版本说明

- 系统环境：Ubuntu 20.04.1 LTS

- FISCO-BCOS >= 2.0.0

- python == 3.6.3、3.7.x

- tensorflow >= 2.0.0

- cmake >= 3.10

- pandas == 1.2.3

- numpy == 1.19.2

- matplotlib == 3.3.4

- sklearn == 0.24.1

  

### 预编译合约准备

#### 1. 下载FISCO-BCOS源码

```shell
git clone https://github.com/FISCO-BCOS/FISCO-BCOS.git
```

#### 2. 嵌入预编译合约

放置预编译合约文件.cpp和.h文件在 *<working_dir>/FISCO-BCOS/libprecompiled/extension* 目录下

```shell
├── extension
   ├── CMakeLists.txt
   ├── CommitteePrecompiled.cpp
   ├── CommitteePrecompiled.h
   ├── .....
```

#### 3. 分配并注册合约地址

通过修改 *<working_dir>/FISCO-BCOS/cmake/templates/UserPrecompiled.h.in* 文件，在 *registerUserPrecompiled* 函数中注册 *CommitteePrecompiled* 合约地址

```c++
void dev::blockverifier::ExecutiveContextFactory::registerUserPrecompiled(dev::blockverifier::ExecutiveContext::Ptr context)
{
    // Address should in [0x5001,0xffff]
    context->setAddress2Precompiled(
        Address(0x5006), std::make_shared<dev::precompiled::CommitteePrecompiled>());
}
```



### 源码编译

#### 1. 安装依赖

```shell
sudo apt install -y g++ libssl-dev openssl cmake git build-essential autoconf texinfo flex patch bison libgmp-dev zlib1g-dev
```

#### 2. 第三方库嵌入

##### 下载nlohmann

由于该项目需要对复杂数据结构进行序列化和反序列化，因此，该项目采用了第三方(nlohmann) *JSON* 序列化/反序列化库

```shell
cd ~/FISCO-BCOS
git clone https://github.com/nlohmann/json.git
```

##### 修改文件夹名称

```shell
mv json/ libnlohmann_json
```

##### CMakeLists.txt配置

*<working_dir>/FISCO-BCOS/CMakeLists.txt*

```cmake
...
# 以下命令放在add_subdirectory(libnlohmann_json)命令之前
set(JSON_BuildTests OFF CACHE INTERNAL "")
...
# 以下命令放在该文件中add_subdirectory部分的任意位置
add_subdirectory(libnlohmann_json)
...
```

*<working_dir>/FISCO-BCOS/libprecompiled/CMakeLists.txt*

```cmake
# 在target_link_libraries命令中增加nlohmann_json目标链接库，其中“...”代表其他目标链接库
target_link_libraries(precompiled PRIVATE ... nlohmann_json)
```

##### 头文件引入

使用该第三方库时，在.h或者.cpp文件中加入以下语句。本项目在CommitteePrecompiled.h文件中已加入以下语句，无需再次添加。

```c++
#include <libnlohmann_json/single_include/nlohmann/json.hpp>
```

#### 3. 建立build目录

```shell
cd ~/FISCO-BCOS
mkdir -p build && cd build
```

#### 4. 编译

```shell
cmake ..
# 根据机器性能自行选用并行编译
make -j4
# 编译完成后在该目录下 ./bin 文件夹生成以下区块链运行所需的可执行程序 fisco-bcos
```

> - 编译问题参考链接：https://fisco-bcos-documentation.readthedocs.io/zh_CN/latest/docs/faq/compile.html
> - 如果因为网络问题导致长时间无法下载依赖库，请尝试从 https://gitee.com/FISCO-BCOS/LargeFiles/tree/master/libs 下载，放在<working_dir>/FISCO-BCOS/deps/src/



### 搭建单群组多节点联盟链

#### 1. 安装依赖

```shell
sudo apt install -y openssl curl
```

#### 2. 下载建链脚本

```shell
# 创建操作目录
cd ~ && mkdir -p fisco && cd fisco
# 下载脚本
curl -LO https://github.com/FISCO-BCOS/FISCO-BCOS/releases/download/v2.7.2/build_chain.sh && chmod u+x build_chain.sh
```

#### 3. 搭建单群组4节点联盟链

在 ip 为127.0.0.1下建立4个节点的区块链网络，每个节点后台运行fisco-bcos可执行程序，每个节点占用三个端口，节点的p2p，channel，jsonrpc起始端口分别为：30300，20200，8545。请确保端口没有被占用。

```shell
bash build_chain.sh -l 127.0.0.1:4 -p 30300,20200,8545 -e ~/FISCO-BCOS/build/bin/fisco-bcos
```

#### 4. 启动联盟链/停止联盟链

启动所有节点

```shell
bash nodes/127.0.0.1/start_all.sh
```

停止所有节点

```shell
bash nodes/127.0.0.1/stop_all.sh
```



### 客户端配置

#### 1. Python SDK安装与配置

##### 软件依赖

```shell
sudo apt install -y zlib1g-dev libffi6 libffi-dev wget git
```

##### 拉取官方源代码

```shell
# 第一步：
git clone https://github.com/FISCO-BCOS/python-sdk
# 第二步：
# 在官方源代码中增加本项目中python-sdk文件夹下相关文件到相应的地方
```

##### 配置环境

若已有符合版本要求的python环境，则直接进入该环境。本项目直接进入符合版本要求，且配置了tensorflow框架的python虚拟环境

```shell
# source activate (env_name)
source activate tensorflow-gpu
cd python-sdk
```

若常用python环境不符合版本要求，请重新安装。或不存在已有python环境，则输入以下命令

```shell
cd python-sdk && bash init_env.sh -p
# 激活python-sdk虚拟环境
source ~/.bashrc && pyenv activate python-sdk && pip install --upgrade pip
```

##### 安装依赖

```shell
pip install -r requirements.txt
```

##### 初始化配置

```shell
# 使用模板的默认配置初始化客户端配置
cp client_config.py.template client_config.py
# 根据配置下载solc编译器
bash init_env.sh -i
```

##### 配置Channel通信协议

Channel协议是FISCO BCOS特有的加密通信协议，具有良好的机密性。根据建链时节点配置的Channel端口，修改 *<working_dir>/python-sdk/client_config.py* 文件

```python
# 本项目建链时，ip=127.0.0.1，节点0的channel_listen_port=20200
channel_host = "127.0.0.1"
channel_port = 20200
```

配置证书

```shell
cp ~/fisco/nodes/127.0.0.1/sdk/* bin/
```

配置证书路径，修改 *<working_dir>/python-sdk/client_config.py* 文件

```python
# 根据从节点拷贝的sdk证书路径，设置sdk证书和私钥路径
channel_node_cert = "bin/sdk.crt"
channel_node_key = "bin/sdk.key"
```

#### 2. 定义预编译合约接口

在 *<working_dir>/python-sdk/contracts* 目录下创建 *CommitteePrecompiled.sol* 文件声明合约接口

```solidity
pragma solidity ^0.4.24;

contract CommitteePrecompiled{
    function RegisterNode() public;                                   //节点注册
    function QueryState() public view returns(string, int);     	  //查询状态
    function QueryGlobalModel() public view returns(string, int);     //获取全局模型
    function UploadLocalUpdate(string update, int256 epoch) public;   //上传本地模型
    function UploadScores(int256 epoch, string scores) public;        //上传评分
    function QueryAllUpdates() public view returns(string);           //获取所有本地模型
}
```

#### 3. 测试账户生成

##### 获取脚本

```shell
cd ~/python-sdk/bin
curl -LO https://raw.githubusercontent.com/FISCO-BCOS/LargeFiles/master/tools/get_account.sh && chmod u+x get_account.sh && bash get_account.sh -h
```

##### 生成PEM格式账户私钥（若需批量生成，忽略此步骤）

```shell
bash get_account.sh
```

##### 批量生成测试账户私钥

创建 *<working_dir>/python-sdk/bin/get_batch_accounts.sh* 脚本文件，调用get_account.sh批量生成20个测试账户

```shell
bash get_batch_accounts.sh -n 20
```

#### 4. 用户代码准备

##### 加载abi定义

```python
from client.datatype_parser import DatatypeParser
from client.common.compiler import Compiler
from client_config import client_config
...

# 编译合约接口
if os.path.isfile(client_config.solc_path) or os.path.isfile(client_config.solcjs_path):
    Compiler.compile_file("contracts/CommitteePrecompiled.sol")
# 从文件加载abi定义
abi_file = "contracts/CommitteePrecompiled.abi"
data_parser = DatatypeParser()
data_parser.load_abi_file(abi_file)
contract_abi = data_parser.contract_abi
```

##### 合约地址

```python
# 定义合约地址
to_address = "0x0000000000000000000000000000000000005006"
```

##### 初始化BCOS客户端

```python
from client.bcosclient import BcosClient
from client.bcoserror import BcosException, BcosError
...

try:
    ...
    # 实例化
    client = BcosClient()
    # 为了模拟多个客户端，加载创建的测试账户
    client.set_from_account_signer(node_id)
    ...
except Exception as e:
    client.finish()
    traceback.print_exc()
```

在 *<working_dir>/python-sdk/client/bcosclient.py* 加入以下函数

```python
class BcosClient:
    ...
    def set_from_account_signer(self, node_id):
        self.key_file = "{}/{}.pem".format(client_config.account_keyfile_path,
                                           node_id)
        self.default_from_account_signer = Signer_ECDSA.from_key_file(
            self.key_file, None)
    ...
```

#### 5. 接口调用

##### 节点注册

```python
receipt = client.sendRawTransactionGetReceipt(to_address, contract_abi, "RegisterNode", [])
```

##### 节点查询状态

```python
role, global_epoch =  client.call(to_address, contract_abi, "QueryState")
```

##### 获取全局模型

```python
model, epoch = client.call(to_address, contract_abi, "QueryGlobalModel")
```

##### 上传本地更新

```python
receipt = client.sendRawTransactionGetReceipt(to_address, contract_abi, "UploadLocalUpdate", [update_model, epoch])
```

##### 上传评分

```python
receipt = client.sendRawTransactionGetReceipt(to_address, contract_abi, "UploadScores", [epoch, serialize(scores)])
```

##### 获取所有本地更新

```python
res = client.call(to_address, contract_abi, "QueryAllUpdates")
updates = res[0]
```

#### 6. Demo运行结果

运行 *<working_dir>/python-sdk/main.py* 文件

##### 运行前

<img src=".\imgs\run_before.png" alt="run_before" style="zoom:80%;" />

##### 运行时

<img src=".\imgs\runtime.jpg" alt="runtime" style="zoom:80%;" />

