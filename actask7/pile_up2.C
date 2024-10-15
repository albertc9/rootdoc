
void pile_up2() {
    // 初始化参数
    const long long total_time = 2000000000; // 总模拟时间（单位：微秒，2000 秒）
    const double event_rate = 60;            // 事件发生率（单位：Hz）
    const double lambda = event_rate * 1e-6; // 将泊松分布参数从秒转换到微秒
    const int dead_time = 3000;              // 死时间（单位：微秒，3 ms）

    long long pile_up_count = 0; // 堆积事件数量
    long long normal_count = 0;  // 正常事件数量
    long long current_time = 0;  // 当前时间（微秒）

    // 设置随机数生成器
    TRandom3 randGen(0); // 使用默认种子（0）以获得不同运行的随机结果

    // 初始化工作状态数组（用于死时间标记）
    bool* is_dead = new bool[total_time](); // 初始值为false，表示没有死时间
                                            // 使用动态分配数组以确保内存充足

    for (current_time = 0; current_time < total_time; ++current_time) {
        int event_occurrence = randGen.Poisson(lambda);
        
        if (event_occurrence > 0) {
            if (!is_dead[current_time]) {
                // 如果该事件不在死时间内，计入正常事件
                normal_count++;
                
                // 将接下来的 3,000 微秒（3 ms）标记为死时间
                for (int t = 0; t < dead_time && current_time + t < total_time; ++t) {
                    is_dead[current_time + t] = true;
                }
            } else {
                // 如果该事件在死时间内，计入堆积事件
                pile_up_count++;
            }
        }
    }

    // 计算并输出事件堆积率
    double pile_up_rate = static_cast<double>(pile_up_count) / (pile_up_count + normal_count);
    
    std::cout << "Total Events: " << normal_count + pile_up_count << std::endl;
    std::cout << "Normal Events: " << normal_count << std::endl;
    std::cout << "Pile-up Events: " << pile_up_count << std::endl;
    std::cout << "Pile-up Rate: " << pile_up_rate << std::endl;

    // 清理动态分配的内存
    delete[] is_dead;
}