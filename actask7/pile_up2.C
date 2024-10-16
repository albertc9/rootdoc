
void pile_up2() {
    // 初始化参数
    long long resolution = 1;
    const long long total_time = 5000000000; // 总模拟时间（单位：微秒，5000 秒）
    const double event_rate = 60;            // 事件发生率（单位：Hz）
    const double lambda = event_rate * 1e-6 * resolution; // 将泊松分布参数从秒转换到指定分辨率
    const int dead_time = 3000 / resolution;              // 死时间（单位：分辨率单位）

    long long pile_up_count = 0;   // 堆积事件数量
    long long normal_count = 0;  // 正常事件数量
    long long current_time = 0;  // 当前时间（分辨率单位）
    long long dead_time_with_pile_up = 0; // 发生堆积的死时间个数

    // 设置随机数生成器
    TRandom3 randGen(0); // 使用默认种子（0）以获得不同运行的随机结果

    // 初始化工作状态数组（用于死时间标记）
    bool* is_dead = new bool[total_time / resolution](); // 初始值为false，表示没有死时间
    bool* has_pile_up = new bool[total_time / resolution](); // 初始值为false，表示没有堆积事件

    for (current_time = 0; current_time < total_time / resolution; ++current_time) {
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
                // 如果当前时间段还没有记录堆积事件，则记录一次
                if (!has_pile_up[current_time]) {
                    dead_time_with_pile_up++;
                    // 标记当前时间段已经记录过堆积事件
                    for (int t = 0; t < dead_time && current_time + t < total_time; ++t) {
                        has_pile_up[current_time + t] = true;
                    }
                }
            }
        }
    }

    // 计算并输出事件堆积率
    double pile_up_rate = static_cast<double>(pile_up_count) / (pile_up_count + normal_count);
    std::cout << "堆积率: " << pile_up_rate << std::endl;

    // 计算并输出事件堆积概率
    double pile_up_probability = static_cast<double>(dead_time_with_pile_up) / normal_count;
    std::cout << "分辨率: " << resolution << " us" << std::endl;
    std::cout << "堆积概率: " << pile_up_probability << std::endl;
    std::cout << "总事件数: " << normal_count + pile_up_count << std::endl;
    std::cout << "正常事件数: " << normal_count << std::endl;
    std::cout << "堆积事件数: " << pile_up_count << std::endl;

    // 清理动态分配的内存
    delete[] is_dead;
    delete[] has_pile_up;
}