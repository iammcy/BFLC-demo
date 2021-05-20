pragma solidity ^0.4.24;

contract CommitteePrecompiled{
    function RegisterNode() public;                                          //节点注册
    function QueryState() public view returns(string, int);     //查询状态
    function QueryGlobalModel() public view returns(string, int);           //获取全局模型
    function UploadLocalUpdate(string update, int256 epoch) public;            //上传本地模型
    function UploadScores(int256 epoch, string scores) public;                 //上传评分
    function QueryAllUpdates() public view returns(string);                 //获取所有本地模型
}