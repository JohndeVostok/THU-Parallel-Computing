# Parallel Computing
马子轩 2019211119

清华大学 计算机系

## 一. 实验环境
    我们的实验在集群gorgon上执行。gorgon集群包含9个节点。每个节点的配置如下

    CPU: Dual Intel(R) Xeon(R) CPU E5-2670 v3 @ 2.30GHz
    Memory: 128GB
    GPU: Nvidia Tesla V100
    NIC: ConnectX-4 EDR 100Gb/s InfiniBand MCX455A + EDR Switch

    OS: Ubuntu 16.04.4 LTS
    MPI: OpenMPI 3.0.0

## 二. MPI实验
### 实验内容
    1. 生成一组数据，分配到不同的进程中。
    2. 通过并行IO将数据写入文件
    3. 将数据通过通讯传输到0号进程
    4. 比较数据

### 实现
    1. 我们通过MPI_FILE_write_at实现写入文件
    2. 通讯使用MPI_Gatherv实现收取数据
    3. 最后通过串行IO读取数据进行验证

### 执行
    程序代码: parallel.cpp
    
    ```shell
        mpicxx --std=c++11 parallel.cpp -o parallel
        mpirun -np 8 parallel
    ```

    注意: 程序由于需要写文件对比，因此执行只能在一台机器上执行,一定不可以在多个机器上执行。

    输出"OK!"代表程序运行正确。

### 结论
    
    验证成功，实验完成
