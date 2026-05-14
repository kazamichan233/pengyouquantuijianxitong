#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <direct.h> // _getcwd
#include <io.h> // _access

#define MAX_USERS 100
#define MAX_PRODUCTS 50

// 用户信息结构体
typedef struct {
    char username[50]; // 用户名
    char password[50]; // 密码
    int friends[MAX_USERS]; // 朋友圈（存储好友的用户ID）
    int purchased[MAX_PRODUCTS]; // 购买的商品（存储商品ID及数量）
} User;

// 商品信息结构体
typedef struct {
    char name[50]; // 商品名
    int purchaseCount; // 购买次数
} Product;

// 定义一个结构体来封装全局变量
typedef struct {
    User users[MAX_USERS];
    Product products[MAX_PRODUCTS];
    int userCount;
    int productCount;
    int currentUserIndex; // -1 表示未登录
    char dataFilePath[_MAX_PATH];
} AppState;

// 初始化全局状态变量
AppState appState = {
    .userCount = 0,
    .productCount = 0,
    .currentUserIndex = -1,
    .dataFilePath = "data.txt"
};

// ---------- 输入辅助函数（使用 fgets，拒绝空输入或仅空白） ----------
static void readLine(char* buf, int size) {
    if (!fgets(buf, size, stdin)) {
        buf[0] = '\0';
        return;
    }

    // --- 核心修复：清理缓冲区残留 ---
    // 检查读取的字符串长度是否接近缓冲区上限，且末尾没有换行符
    // 这通常意味着输入被截断了，缓冲区里还有剩余字符
    int len = strlen(buf);
    if (len > 0 && buf[len - 1] != '\n') {
        // 循环读取并丢弃缓冲区中的剩余字符，直到遇到换行符或文件结束
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    // ---------------------------------

    // 2. 去掉结尾的换行和回车
    size_t len2 = strcspn(buf, "\r\n");
    buf[len2] = '\0';
}

// 去除字符串两端空白字符（空格、制表、回车等）
static void trim_whitespace(char* s) {
    if (!s || !*s) return;
    char* start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[len - 1] = '\0';
        len--;
    }
}

static void readNonEmptyString(const char* prompt, char* out, int size) {
    while (1) {
        printf("%s", prompt);
        readLine(out, size);
        trim_whitespace(out);
        if (out[0] != '\0') {
            if (strlen(out) >= size) {
                printf("输入过长，请限制在 %d 个字符以内！\n", size - 1);
                continue;
            }
            return;
        }
        printf("输入不能为空或仅包含空白字符，请重试！\n");
    }
}

static int readIntWithPrompt(const char* prompt) {
    char line[128];
    while (1) {
        printf("%s", prompt);
        readLine(line, sizeof(line));
        trim_whitespace(line);
        if (line[0] == '\0') {
            printf("输入不能为空或仅包含空白字符，请重试！\n");
            continue;
        }
        char* endptr = NULL;
        long val = strtol(line, &endptr, 10);
        if (endptr == line || *endptr != '\0') {
            printf("输入无效，请输入整数！\n");
            continue;
        }
        if (val < INT_MIN || val > INT_MAX) {
            printf("输入超出整数范围，请重试！\n");
            continue;
        }
        return (int)val;
    }
}
// -----------------------------------------------------------------%

// 添加用户注册功能
void registerUser() {
    if (appState.userCount >= MAX_USERS) {
        printf("用户数量已达上限，无法注册新用户！\n");
        return;
    }

    char username[50], password[50];

    readNonEmptyString("请输入用户名：", username, sizeof(username));

    // 检查用户名是否重复
    char lowerUsername[50], lowerStoredUsername[50];
    for (int i = 0; i < appState.userCount; i++) {
        strncpy(lowerUsername, username, 50);
        strncpy(lowerStoredUsername, appState.users[i].username, 50);
        for (int j = 0; j < 50; j++) {
            lowerUsername[j] = tolower((unsigned char)lowerUsername[j]);
            lowerStoredUsername[j] = tolower((unsigned char)lowerStoredUsername[j]);
        }

        if (strncmp(lowerStoredUsername, lowerUsername, 50) == 0) {
            printf("用户名已存在，请选择其他用户名！\n");
            return;
        }
    }

    readNonEmptyString("请输入密码：", password, sizeof(password));

    // 添加新用户
    strcpy(appState.users[appState.userCount].username, username);
    strcpy(appState.users[appState.userCount].password, password);
    memset(appState.users[appState.userCount].friends, 0, sizeof(appState.users[appState.userCount].friends));
    memset(appState.users[appState.userCount].purchased, 0, sizeof(appState.users[appState.userCount].purchased));
    appState.userCount++;

    printf("用户注册成功！\n");
}

// 修改登录功能，记录当前登录用户
void loginUser() {
    char username[50], password[50];
    
    readNonEmptyString("请输入用户名：", username, sizeof(username));
    readNonEmptyString("请输入密码：", password, sizeof(password));

    // 验证用户名和密码
    char lowerUsername[50], lowerStoredUsername[50];
    for (int i = 0; i < appState.userCount; i++) {
        strncpy(lowerUsername, username, 50);
        strncpy(lowerStoredUsername, appState.users[i].username, 50);
        for (int j = 0; j < 50; j++) {
            lowerUsername[j] = tolower((unsigned char)lowerUsername[j]);
            lowerStoredUsername[j] = tolower((unsigned char)lowerStoredUsername[j]);
        }

        if (strncmp(lowerStoredUsername, lowerUsername, 50) == 0 && strcmp(appState.users[i].password, password) == 0) {
            appState.currentUserIndex = i; // 记录当前登录用户索引
            printf("登录成功！欢迎您，%s！\n", appState.users[i].username);
            return;
        }
    }

    printf("用户名或密码错误，请重试！\n");
}

// 添加用户信息管理功能
void addUser() {
    registerUser();
}

// 通用函数：查找用户索引（不区分大小写）
int findUserIndex(const char* username) {
    char lowerUsername[50], lowerStoredUsername[50];
    for (int i = 0; i < appState.userCount; i++) {
        // 转换为小写
        strncpy(lowerUsername, username, 50);
        strncpy(lowerStoredUsername, appState.users[i].username, 50);
        for (int j = 0; j < 50; j++) {
            lowerUsername[j] = tolower((unsigned char)lowerUsername[j]);
            lowerStoredUsername[j] = tolower((unsigned char)lowerStoredUsername[j]);
        }

        if (strncmp(lowerStoredUsername, lowerUsername, 50) == 0) {
            return i; // 返回用户索引
        }
    }
    return -1; // 未找到返回-1
}

// 通用函数：查找商品索引（不区分大小写）
int findProductIndex(const char* name) {
    char lowerName[50], lowerStoredName[50];
    for (int i = 0; i < appState.productCount; i++) {
        // 转换为小写
        strncpy(lowerName, name, 50);
        strncpy(lowerStoredName, appState.products[i].name, 50);
        for (int j = 0; j < 50; j++) {
            lowerName[j] = tolower((unsigned char)lowerName[j]);
            lowerStoredName[j] = tolower((unsigned char)lowerStoredName[j]);
        }

        if (strncmp(lowerStoredName, lowerName, 50) == 0) {
            return i; // 返回商品索引
        }
    }
    return -1; // 未找到返回-1
}

// 修改用户信息功能，使用通用函数
void modifyUser() {
    char username[50];
    readNonEmptyString("请输入要修改的用户名：", username, sizeof(username)); // 限制输入长度为 49

    int userIndex = findUserIndex(username);
    if (userIndex != -1) {
        readNonEmptyString("请输入新密码：", appState.users[userIndex].password, sizeof(appState.users[userIndex].password)); // 限制输入长度为 49
        printf("用户信息修改成功！\n");
    }
    else {
        printf("未找到该用户！\n");
    }
}

// 删除用户功能，使用通用函数
void deleteUser() {
    char username[50];
    readNonEmptyString("请输入要删除的用户名：", username, sizeof(username));

    int userIndex = findUserIndex(username);
    if (userIndex != -1) {
        // 清理其他用户的好友关系
        for (int i = 0; i < appState.userCount; i++) {
            appState.users[i].friends[userIndex] = 0;
        }

        // 删除用户并移动数组
        for (int j = userIndex; j < appState.userCount - 1; j++) {
            appState.users[j] = appState.users[j + 1];
            // 更新好友关系
            for (int k = 0; k < appState.userCount; k++) {
                appState.users[k].friends[j] = appState.users[k].friends[j + 1];
            }
        }
        appState.userCount--;
        printf("用户删除成功！\n");
    } else {
        printf("未找到该用户！\n");
    }
}

// 添加商品信息管理功能
void addProduct() {
    if (appState.productCount >= MAX_PRODUCTS) {
        printf("商品数量已达上限，无法添加新商品！\n");
        return;
    }

    char name[50];
    readNonEmptyString("请输入商品名：", name, sizeof(name)); // 限制输入长度为 49

    // 检查商品是否已存在
    for (int i = 0; i < appState.productCount; i++) {
        if (strcmp(appState.products[i].name, name) == 0) {
            printf("商品已存在！\n");
            return;
        }
    }

    strcpy(appState.products[appState.productCount].name, name);
    appState.products[appState.productCount].purchaseCount = 0;
    appState.productCount++;

    printf("商品添加成功！\n");
}

// 修改商品信息功能，使用通用函数
void modifyProduct() {
    char name[50];
    readNonEmptyString("请输入要修改的商品名：", name, sizeof(name)); // 限制输入长度为 49

    int productIndex = findProductIndex(name);
    if (productIndex != -1) {
        int newCount = readIntWithPrompt("请输入新的购买次数：");
        appState.products[productIndex].purchaseCount = newCount;
        printf("商品信息修改成功！\n");
    }
    else {
        printf("未找到该商品！\n");
    }
}

// 删除商品功能，使用通用函数
void deleteProduct() {
    char name[50];
    readNonEmptyString("请输入要删除的商品名：", name, sizeof(name)); // 限制输入长度为 49

    int productIndex = findProductIndex(name);
    if (productIndex != -1) {
        // 清理所有用户的购买记录
        for (int i = 0; i < appState.userCount; i++) {
            appState.users[i].purchased[productIndex] = 0;
        }

        // 删除商品并移动数组
        for (int j = productIndex; j < appState.productCount - 1; j++) {
            appState.products[j] = appState.products[j + 1];
        }
        appState.productCount--;
        printf("商品删除成功！\n");
    }
    else {
        printf("未找到该商品！\n");
    }
}

// 添加用户查询功能
void queryUser() {
    char username[50];
    readNonEmptyString("请输入要查询的用户名：", username, sizeof(username)); // 限制输入长度为 49

    for (int i = 0; i < appState.userCount; i++) {
        if (strcmp(appState.users[i].username, username) == 0) {
            printf("用户名：%s\n", appState.users[i].username);
            printf("购买的商品：");
            for (int j = 0; j < MAX_PRODUCTS; j++) {
                if (appState.users[i].purchased[j] > 0) {
                    printf("商品ID %d (数量 %d) ", j, appState.users[i].purchased[j]);
                }
            }
            printf("\n");
            return;
        }
    }

    printf("未找到该用户！\n");
}

// 添加商品查询功能
void queryProduct() {
    char name[50];
    readNonEmptyString("请输入要查询的商品名：", name, sizeof(name)); // 限制输入长度为 49

    for (int i = 0; i < appState.productCount; i++) {
        if (strcmp(appState.products[i].name, name) == 0) {
            printf("商品名：%s\n", appState.products[i].name);
            printf("购买次数：%d\n", appState.products[i].purchaseCount);
            return;
        }
    }

    printf("未找到该商品！\n");
}

// 添加商品排序功能
// 快速排序辅助函数
void quickSort(Product arr[], int left, int right) {
    if (left >= right) return;

    int pivot = arr[right].purchaseCount;
    int i = left - 1;
    for (int j = left; j < right; j++) {
        if (arr[j].purchaseCount > pivot) {
            i++;
            Product temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    Product temp = arr[i + 1];
    arr[i + 1] = arr[right];
    arr[right] = temp;

    quickSort(arr, left, i);
    quickSort(arr, i + 2, right);
}

void sortProducts() {
    if (appState.productCount > 0) quickSort(appState.products, 0, appState.productCount - 1);

    printf("商品已按购买次数排序：\n");
    for (int i = 0; i < appState.productCount; i++) {
        printf("商品名：%s，购买次数：%d\n", appState.products[i].name, appState.products[i].purchaseCount);
    }
}

// 替换推荐算法中的冒泡排序为快速排序
static void quickSortRecommendations(int arr[], int indices[], int left, int right) {
    if (left >= right) return;

    int pivot = arr[indices[right]];
    int i = left - 1;
    for (int j = left; j < right; j++) {
        if (arr[indices[j]] > pivot) {
            i++;
            int temp = indices[i];
            indices[i] = indices[j];
            indices[j] = temp;
        }
    }

    int temp = indices[i + 1];
    indices[i + 1] = indices[right];
    indices[right] = temp;

    quickSortRecommendations(arr, indices, left, i);
    quickSortRecommendations(arr, indices, i + 2, right);
}

void recommendProducts(int userId) {
    if (userId < 0 || userId >= appState.userCount) {
        printf("无效的用户ID！\n");
        return;
    }

    int recommendation[MAX_PRODUCTS] = { 0 }; // 推荐商品的购买次数统计

    // 遍历用户的朋友圈 —— 只遍历已注册用户范围
    for (int i = 0; i < appState.userCount; i++) {
        if (appState.users[userId].friends[i]) { // 如果是朋友
            int friendId = i; // 获取朋友的用户ID

            // 累加朋友购买的商品
            for (int j = 0; j < MAX_PRODUCTS; j++) {
                recommendation[j] += appState.users[friendId].purchased[j];
            }
        }
    }

    // 排除用户自己已经购买的商品
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (appState.users[userId].purchased[i] > 0) {
            recommendation[i] = 0; // 将已购买商品的推荐指数置为0
        }
    }

    // 按购买次数排序推荐商品
    int sortedIndices[MAX_PRODUCTS]; // 存储商品索引的数组
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        sortedIndices[i] = i;
    }

    quickSortRecommendations(recommendation, sortedIndices, 0, MAX_PRODUCTS - 1);

    // 检查是否有有效推荐
    int hasRecommendation = 0;
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (recommendation[sortedIndices[i]] > 0) {
            hasRecommendation = 1;
            break;
        }
    }

    if (!hasRecommendation) {
        printf("暂无推荐商品！\n");
        return;
    }

    // 检查是否有有效推荐
    int hasRecommendation = 0;
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (recommendation[sortedIndices[i]] > 0) {
            hasRecommendation = 1;
            break;
        }
    }

    if (!hasRecommendation) {
        printf("暂无推荐商品！\n");
        return;
    }

    // 输出推荐商品
    printf("为用户推荐的商品：\n");
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (recommendation[sortedIndices[i]] > 0) {
            printf("商品ID %d，推荐指数 %d\n", sortedIndices[i], recommendation[sortedIndices[i]]);
        }
    }
}

// 添加文件存储功能
void saveToFile() {
    while (1) {
        FILE* file = fopen(appState.dataFilePath, "w");
        if (!file) {
            int err = errno;
            printf("无法保存数据到文件：%s，errno=%d (%s)\n", appState.dataFilePath, err, strerror(err));
            printf("请重新设置数据文件路径。\n");
            setFilePath();
            continue; // 重新尝试保存
        }

        // 写入数据（保持原有格式）
        if (fprintf(file, "%d\n", appState.userCount) < 0) {
            printf("保存用户数量失败！\n");
            fclose(file);
            return;
        }

        for (int i = 0; i < appState.userCount; i++) {
            if (fprintf(file, "%s %s\n", appState.users[i].username, appState.users[i].password) < 0) {
                printf("保存用户信息失败！\n");
                fclose(file);
                return;
            }
            for (int j = 0; j < appState.userCount; j++) {
                if (fprintf(file, "%d ", appState.users[i].friends[j]) < 0) {
                    printf("保存用户好友信息失败！\n");
                    fclose(file);
                    return;
                }
            }
            fprintf(file, "\n");
            for (int j = 0; j < appState.productCount; j++) {
                if (fprintf(file, "%d ", appState.users[i].purchased[j]) < 0) {
                    printf("保存用户购买记录失败！\n");
                    fclose(file);
                    return;
                }
            }
            fprintf(file, "\n");
        }

        if (fprintf(file, "%d\n", appState.productCount) < 0) {
            printf("保存商品数量失败！\n");
            fclose(file);
            return;
        }

        for (int i = 0; i < appState.productCount; i++) {
            if (fprintf(file, "%s %d\n", appState.products[i].name, appState.products[i].purchaseCount) < 0) {
                printf("保存商品信息失败！\n");
                fclose(file);
                return;
            }
        }

        fclose(file);
        printf("数据已成功保存到：%s\n", appState.dataFilePath);
        return;
    }
}

// 添加文件加载功能
// 添加文件加载功能
void loadFromFile() {
    while (1) {
        FILE* file = fopen(appState.dataFilePath, "r");

        // 如果文件不存在，提供选择
        if (!file) {
            printf("未找到数据文件：%s\n", appState.dataFilePath);
            printf("请选择操作：\n");
            printf("1. 重新设置数据文件路径\n");
            printf("2. 冷启动生成默认测试数据\n");

            int choice = readIntWithPrompt("请输入您的选择（1 或 2）：");
            if (choice == 1) {
                printf("请重新设置数据文件路径,或检查是否有读写权限。\n");
                setFilePath();
                continue; // 重新尝试加载
            }
            else if (choice == 2) {
                printf("正在生成默认测试数据...\n");

                // --- 生成默认用户 ---
                appState.userCount = MAX_USERS;
                for (int i = 0; i < appState.userCount; i++) {
                    snprintf(appState.users[i].username, sizeof(appState.users[i].username), "user%d", i + 1);
                    strncpy(appState.users[i].password, "pass", sizeof(appState.users[i].password) - 1);
                    appState.users[i].password[sizeof(appState.users[i].password) - 1] = '\0';
                    memset(appState.users[i].friends, 0, sizeof(appState.users[i].friends));
                    memset(appState.users[i].purchased, 0, sizeof(appState.users[i].purchased));
                }

                // --- 生成默认商品 ---
                appState.productCount = MAX_PRODUCTS;
                for (int i = 0; i < appState.productCount; i++) {
                    snprintf(appState.products[i].name, sizeof(appState.products[i].name), "pro%d", i + 1);
                    appState.products[i].purchaseCount = 0; // 初始化为0
                }

                // --- 建立双向好友关系 ---
                printf("正在建立社交网络...\n");
                for (int i = 0; i < appState.userCount; i++) {
                    // 随机决定该用户的好友数量 (例如 3 到 7 个)
                    int targetFriends = 3 + rand() % 5;
                    int added = 0;

                    while (added < targetFriends) {
                        int friendId = rand() % appState.userCount;

                        // 跳过自己或已添加的情况
                        if (friendId == i || appState.users[i].friends[friendId]) {
                            continue;
                        }

                        // 双向添加好友 (核心逻辑)
                        appState.users[i].friends[friendId] = 1;
                        appState.users[friendId].friends[i] = 1;

                        added++;
                    }
                }

                // --- 生成随机购买记录并同步 purchaseCount ---
                printf("正在生成购买行为数据...\n");
                for (int i = 0; i < appState.userCount; i++) {
                    // 随机决定该用户购买的商品种类数 (例如 2 到 5 种)
                    int boughtTypes = 2 + rand() % 4;

                    for (int j = 0; j < boughtTypes; j++) {
                        int productId = rand() % appState.productCount;
                        // 随机购买数量 (1 到 3 件)
                        int quantity = 1 + rand() % 3;

                        // 更新用户购买记录
                        appState.users[i].purchased[productId] += quantity;

                        // 重点：同步更新商品的总购买次数
                        appState.products[productId].purchaseCount += quantity;
                    }
                }

                printf("默认测试数据生成完毕！\n");
                printf("已生成 %d 个用户和 %d 个商品。\n", appState.userCount, appState.productCount);
                printf("社交关系与购买记录已初始化。\n");

                return; // 直接返回，不进行后续的文件读取
            }
            else {
                printf("无效的选择，请重新输入！\n");
                continue;
            }
        }

        // --- 以下为文件读取逻辑 ---
        int fileUserCount = 0;
        if (fscanf(file, "%d\n", &fileUserCount) != 1) {
            printf("数据文件格式错误：无法读取用户数量！\n");
            fclose(file);
            printf("请重新设置数据文件路径或检查文件是否存在。\n");
            setFilePath();
            continue;
        }

        if (fileUserCount < 0 || fileUserCount > MAX_USERS) {
            printf("数据文件格式错误：用户数量超出范围！\n");
            fclose(file);
            printf("请重新设置数据文件路径或检查文件是否存在。\n");
            setFilePath();
            continue;
        }
        appState.userCount = fileUserCount;

        char line[4096];
        for (int i = 0; i < appState.userCount; i++) {
            // 读取用户名和密码
            if (fscanf(file, "%49s %49s", appState.users[i].username, appState.users[i].password) != 2) {
                printf("数据文件格式错误：无法读取用户信息！\n");
                fclose(file);
                printf("请重新设置数据文件路径或检查文件是否存在。\n");
                setFilePath();
                continue;
            }

            // 清除到行尾并读取好友行
            if (!fgets(line, sizeof(line), file)) {
                printf("数据文件格式错误：无法读取用户好友信息！\n");
                fclose(file);
                printf("请重新设置数据文件路径或检查文件是否存在。\n");
                setFilePath();
                continue;
            }

            // 解析好友
            memset(appState.users[i].friends, 0, sizeof(appState.users[i].friends));
            char* tok = strtok(line, " \t\r\n");
            int idx = 0;
            while (tok != NULL && idx < MAX_USERS) {
                appState.users[i].friends[idx++] = atoi(tok);
                tok = strtok(NULL, " \t\r\n");
            }

            // 读取并解析购买记录
            if (!fgets(line, sizeof(line), file)) {
                printf("数据文件格式错误：无法读取用户购买记录！\n");
                fclose(file);
                printf("请重新设置数据文件路径或检查文件是否存在。\n");
                setFilePath();
                continue;
            }

            memset(appState.users[i].purchased, 0, sizeof(appState.users[i].purchased));
            tok = strtok(line, " \t\r\n");
            idx = 0;
            while (tok != NULL && idx < MAX_PRODUCTS) {
                appState.users[i].purchased[idx++] = atoi(tok);
                tok = strtok(NULL, " \t\r\n");
            }
        }

        // 读取商品数量与商品信息
        int fileProductCount = 0;
        if (fscanf(file, "%d\n", &fileProductCount) != 1) {
            printf("数据文件格式错误：无法读取商品数量！\n");
            fclose(file);
            printf("请重新设置数据文件路径或检查文件是否存在。\n");
            setFilePath();
            continue;
        }

        if (fileProductCount < 0 || fileProductCount > MAX_PRODUCTS) {
            printf("数据文件格式错误：商品数量超出范围！\n");
            fclose(file);
            printf("请重新设置数据文件路径或检查文件是否存在。\n");
            setFilePath();
            continue;
        }
        appState.productCount = fileProductCount;

        for (int i = 0; i < appState.productCount; i++) {
            if (fscanf(file, "%49s %d\n", appState.products[i].name, &appState.products[i].purchaseCount) != 2) {
                printf("数据文件格式错误：无法读取商品信息！\n");
                fclose(file);
                printf("请重新设置数据文件路径或检查文件是否存在。\n");
                setFilePath();
                continue;
            }
        }

        fclose(file);
        printf("数据已成功从文件加载！\n");
        return;
    }
}

// 美化菜单界面，增加更清晰的提示信息
void displayMenu() {
    printf("\n========================================\n");
    printf("          朋友圈商品推荐系统           \n");
    printf("========================================\n");
    if (appState.currentUserIndex != -1) {
        printf("当前登录用户：%s\n", appState.users[appState.currentUserIndex].username);
    }
    else {
        printf("当前未登录\n");
    }
    printf("========================================\n");
    printf("1. 用户注册\n");
    printf("2. 用户登录\n");
    printf("3. 推荐商品\n");
    printf("4. 管理员功能\n");
    printf("5. 管理朋友圈\n");
    printf("6. 保存数据\n");
    printf("7. 设置数据文件路径\n");
    printf("8. 退出系统\n");
    printf("========================================\n");
    printf("请选择操作（输入对应数字）：");
}

// 设置文件路径
    setFilePath() {
    char tempPath[_MAX_PATH];
    while (1) {
        readNonEmptyString("请输入数据文件的绝对路径：", tempPath, sizeof(tempPath));

        // 检查路径是否存在且可写
        FILE* testFile = fopen(tempPath, "a");
        if (testFile) {
            fclose(testFile);
            strncpy(appState.dataFilePath, tempPath, sizeof(appState.dataFilePath));
            appState.dataFilePath[sizeof(appState.dataFilePath) - 1] = '\0'; // 确保字符串以空字符结尾
            printf("数据文件路径已设置为：%s\n", appState.dataFilePath);
            return;
        } else {
            printf("路径无效或不可写，请重新输入！\n");
        }
    }
}

// 管理员功能
void displayAdminMenu() {
    printf("\n========================================\n");
    printf("             管理员功能菜单             \n");
    printf("========================================\n");
    printf("1. 添加用户\n");
    printf("2. 修改用户\n");
    printf("3. 删除用户\n");
    printf("4. 添加商品\n");
    printf("5. 修改商品\n");
    printf("6. 删除商品\n");
    printf("7. 查询用户\n");
    printf("8. 查询商品\n");
    printf("9. 排序商品\n");
    printf("10. 显示当前用户购买数据\n");
    printf("11. 修改当前用户购买数据\n");
    printf("12. 返回主菜单\n");
    printf("========================================\n");
    printf("请选择操作（输入对应数字）：");
}

// 添加管理朋友圈的功能
void displayFriendMenu() {
    printf("\n========================================\n");
    printf("             管理朋友圈菜单             \n");
    printf("========================================\n");
    printf("1. 添加好友\n");
    printf("2. 删除好友\n");
    printf("3. 查询好友\n");
    printf("4. 返回主菜单\n");
    printf("========================================\n"); 
    printf("请选择操作（输入对应数字）：");
}

// 显示当前登录用户的购买数据
void showCurrentUserPurchases() {
    if (appState.currentUserIndex == -1) {
        printf("请先登录！\n");
        return;
    }

    printf("当前用户：%s\n", appState.users[appState.currentUserIndex].username);
    printf("购买的商品：\n");
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (appState.users[appState.currentUserIndex].purchased[i] > 0) {
            printf("商品ID %d (数量 %d)\n", i, appState.users[appState.currentUserIndex].purchased[i]);
        }
    }
}

// 修改当前登录用户的购买数据
void modifyCurrentUserPurchases() {
    if (appState.currentUserIndex == -1) {
        printf("请先登录！\n");
        return;
    }

    if (appState.productCount == 0) {
        printf("当前没有商品可修改！\n");
        return;
    }

    int productId = readIntWithPrompt("请输入要修改的商品ID：");

    if (productId < 0 || productId >= appState.productCount) {
        printf("无效的商品ID！有效范围：0 - %d\n", appState.productCount - 1);
        return;
    }

    int quantity = readIntWithPrompt("请输入购买数量（输入0表示删除购买记录）：");

    if (quantity < 0) {
        printf("购买数量不能为负数！\n");
        return;
    }

    if (quantity > 1000) { // 假设购买次数的上限为1000
        printf("购买数量不能超过1000！\n");
        return;
    }

    // 计算用户原有数量与差值，并同步更新商品的总购买次数
    int oldQty = appState.users[appState.currentUserIndex].purchased[productId];
    appState.users[appState.currentUserIndex].purchased[productId] = quantity;

    int delta = quantity - oldQty;
    appState.products[productId].purchaseCount += delta;
    if (appState.products[productId].purchaseCount < 0) {
        appState.products[productId].purchaseCount = 0; // 防止出现负值
    }

    printf("购买数据已更新！商品 '%s' 的总购买次数为 %d（变化 %+d）。\n",
        appState.products[productId].name, appState.products[productId].purchaseCount, delta);
}

// 添加好友
void addFriend() {
    if (appState.currentUserIndex == -1) {
        printf("请先登录！\n");
        return;
    }

    char friendUsername[50];
    readNonEmptyString("请输入要添加的好友用户名：", friendUsername, sizeof(friendUsername)); // 限制输入长度为 49

    int friendIndex = findUserIndex(friendUsername);
    if (friendIndex == -1) {
        printf("未找到该用户！\n");
        return;
    }

    if (appState.users[appState.currentUserIndex].friends[friendIndex]) {
        printf("该用户已经是您的好友！\n");
        return;
    }

    appState.users[appState.currentUserIndex].friends[friendIndex] = 1;
    printf("好友添加成功！\n");
}

// 删除好友
void removeFriend() {
    if (appState.currentUserIndex == -1) {
        printf("请先登录！\n");
        return;
    }

    char friendUsername[50];
    readNonEmptyString("请输入要删除的好友用户名：", friendUsername, sizeof(friendUsername)); // 限制输入长度为 49

    int friendIndex = findUserIndex(friendUsername);
    if (friendIndex == -1) {
        printf("未找到该用户！\n");
        return;
    }

    if (!appState.users[appState.currentUserIndex].friends[friendIndex]) {
        printf("该用户不是您的好友！\n");
        return;
    }

    appState.users[appState.currentUserIndex].friends[friendIndex] = 0;
    printf("好友删除成功！\n");
}

// 查询好友
void queryFriends() {
    if (appState.currentUserIndex == -1) {
        printf("请先登录！\n");
        return;
    }

    printf("您的好友列表：\n");
    // 只遍历已注册用户范围，避免打印未初始化的用户名
    for (int i = 0; i < appState.userCount; i++) {
        if (appState.users[appState.currentUserIndex].friends[i]) {
            printf("好友用户名：%s\n", appState.users[i].username);
            printf("购买的商品：\n");
            int hasPurchases = 0; // 标记是否有购买记录
            for (int j = 0; j < MAX_PRODUCTS; j++) {
                if (appState.users[i].purchased[j] > 0) {
                    printf("  商品ID %d (数量 %d)\n", j, appState.users[i].purchased[j]);
                    hasPurchases = 1;
                }
            }
            if (!hasPurchases) {
                printf("  无购买记录\n");
            }
        }
    }
}

// 管理员功能
void adminMenu() {
    int adminChoice;
    while (1) {
        displayAdminMenu();
        adminChoice = readIntWithPrompt("");

        switch (adminChoice) {
        case 1:
            addUser();
            break;
        case 2:
            modifyUser();
            break;
        case 3:
            deleteUser();
            break;
        case 4:
            addProduct();
            break;
        case 5:
            modifyProduct();
            break;
        case 6:
            deleteProduct();
            break;
        case 7:
            queryUser();
            break;
        case 8:
            queryProduct();
            break;
        case 9:
            sortProducts();
            break;
        case 10:
            showCurrentUserPurchases();
            break;
        case 11:
            modifyCurrentUserPurchases();
            break;
        case 12:
            return; // 返回主菜单
        default:
            printf("无效的选择，请重新输入！\n");
        }
    }
}

// 管理朋友圈
void friendMenu() {
    int friendChoice;
    while (1) {
        displayFriendMenu();
        friendChoice = readIntWithPrompt("");

        switch (friendChoice) {
        case 1:
            addFriend();
            break;
        case 2:
            removeFriend();
            break;
        case 3:
            queryFriends();
            break;
        case 4:
            return; // 返回主菜单
        default:
            printf("无效的选择，请重新输入！\n");
        }
    }
}

int main() {
    int choice;
    loadFromFile(); // 启动时加载数据

    while (1) {
        displayMenu();
        choice = readIntWithPrompt("");

        switch (choice) {
        case 1:
            registerUser();
            break;
        case 2:
            loginUser();
            break;
        case 3:
            if (appState.currentUserIndex == -1) {
                printf("请先登录！\n");
            }
            else {
                recommendProducts(appState.currentUserIndex);
            }
            break;
        case 4:
            adminMenu();
            break;
        case 5:
            friendMenu();
            break;
        case 6:
            saveToFile();
            break;
        case 7:
            setFilePath();
            break;
        case 8:
            saveToFile();
            printf("退出系统！\n");
            return 0;
        default:
            printf("无效的选择，请重新输入！\n");
        }
    }
}
