#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

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

// 全局变量
User users[MAX_USERS];
Product products[MAX_PRODUCTS];
int userCount = 0;
int productCount = 0;
int currentUserIndex = -1; // -1 表示未登录

// 添加用户注册功能
void registerUser() {
    if (userCount >= MAX_USERS) {
        printf("用户数量已达上限，无法注册新用户！\n");
        return;
    }

    char username[50], password[50];
    printf("请输入用户名：");
    scanf("%s", username);

    // 检查用户名是否重复
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("用户名已存在，请选择其他用户名！\n");
            return;
        }
    }

    printf("请输入密码：");
    scanf("%s", password);

    // 添加新用户
    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    memset(users[userCount].friends, 0, sizeof(users[userCount].friends));
    memset(users[userCount].purchased, 0, sizeof(users[userCount].purchased));
    userCount++;

    printf("用户注册成功！\n");
}

// 修改登录功能，记录当前登录用户
void loginUser() {
    char username[50], password[50];
    printf("请输入用户名：");
    scanf("%s", username);
    printf("请输入密码：");
    scanf("%s", password);

    // 验证用户名和密码
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            currentUserIndex = i; // 记录当前登录用户索引
            printf("登录成功！欢迎您，%s！\n", username);
            return;
        }
    }

    printf("用户名或密码错误，请重试！\n");
}

// 添加用户信息管理功能
void addUser() {
    registerUser();
}

// 提取重复代码为通用函数，减少冗余

// 通用函数：查找用户索引
int findUserIndex(const char *username) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return i; // 返回用户索引
        }
    }
    return -1; // 未找到返回-1
}

// 通用函数：查找商品索引
int findProductIndex(const char *name) {
    for (int i = 0; i < productCount; i++) {
        if (strcmp(products[i].name, name) == 0) {
            return i; // 返回商品索引
        }
    }
    return -1; // 未找到返回-1
}

// 修改用户信息功能，使用通用函数
void modifyUser() {
    char username[50];
    printf("请输入要修改的用户名：");
    scanf("%s", username);

    int userIndex = findUserIndex(username);
    if (userIndex != -1) {
        printf("请输入新密码：");
        scanf("%s", users[userIndex].password);
        printf("用户信息修改成功！\n");
    } else {
        printf("未找到该用户！\n");
    }
}

// 删除用户功能，使用通用函数
void deleteUser() {
    char username[50];
    printf("请输入要删除的用户名：");
    scanf("%s", username);

    int userIndex = findUserIndex(username);
    if (userIndex != -1) {
        for (int j = userIndex; j < userCount - 1; j++) {
            users[j] = users[j + 1];
        }
        userCount--;
        printf("用户删除成功！\n");
    } else {
        printf("未找到该用户！\n");
    }
}

// 添加商品信息管理功能
void addProduct() {
    if (productCount >= MAX_PRODUCTS) {
        printf("商品数量已达上限，无法添加新商品！\n");
        return;
    }

    char name[50];
    printf("请输入商品名：");
    scanf("%s", name);

    // 检查商品是否已存在
    for (int i = 0; i < productCount; i++) {
        if (strcmp(products[i].name, name) == 0) {
            printf("商品已存在！\n");
            return;
        }
    }

    strcpy(products[productCount].name, name);
    products[productCount].purchaseCount = 0;
    productCount++;

    printf("商品添加成功！\n");
}

// 修改商品信息功能，使用通用函数
void modifyProduct() {
    char name[50];
    printf("请输入要修改的商品名：");
    scanf("%s", name);

    int productIndex = findProductIndex(name);
    if (productIndex != -1) {
        printf("请输入新的购买次数：");
        scanf("%d", &products[productIndex].purchaseCount);
        printf("商品信息修改成功！\n");
    } else {
        printf("未找到该商品！\n");
    }
}

// 删除商品功能，使用通用函数
void deleteProduct() {
    char name[50];
    printf("请输入要删除的商品名：");
    scanf("%s", name);

    int productIndex = findProductIndex(name);
    if (productIndex != -1) {
        for (int j = productIndex; j < productCount - 1; j++) {
            products[j] = products[j + 1];
        }
        productCount--;
        printf("商品删除成功！\n");
    } else {
        printf("未找到该商品！\n");
    }
}

// 添加用户查询功能
void queryUser() {
    char username[50];
    printf("请输入要查询的用户名：");
    scanf("%s", username);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("用户名：%s\n", users[i].username);
            printf("购买的商品：");
            for (int j = 0; j < MAX_PRODUCTS; j++) {
                if (users[i].purchased[j] > 0) {
                    printf("商品ID %d (数量 %d) ", j, users[i].purchased[j]);
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
    printf("请输入要查询的商品名：");
    scanf("%s", name);

    for (int i = 0; i < productCount; i++) {
        if (strcmp(products[i].name, name) == 0) {
            printf("商品名：%s\n", products[i].name);
            printf("购买次数：%d\n", products[i].purchaseCount);
            return;
        }
    }

    printf("未找到该商品！\n");
}

// 添加商品排序功能
void sortProducts() {
    for (int i = 0; i < productCount - 1; i++) {
        for (int j = 0; j < productCount - i - 1; j++) {
            if (products[j].purchaseCount < products[j + 1].purchaseCount) {
                Product temp = products[j];
                products[j] = products[j + 1];
                products[j + 1] = temp;
            }
        }
    }

    printf("商品已按购买次数排序：\n");
    for (int i = 0; i < productCount; i++) {
        printf("商品名：%s，购买次数：%d\n", products[i].name, products[i].purchaseCount);
    }
}

// 优化推荐算法，排除用户已购买的商品，并增加注释
void recommendProducts(int userId) {
    if (userId < 0 || userId >= userCount) {
        printf("无效的用户ID！\n");
        return;
    }

    int recommendation[MAX_PRODUCTS] = {0}; // 推荐商品的购买次数统计

    // 遍历用户的朋友圈
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[userId].friends[i]) { // 如果是朋友
            int friendId = i; // 获取朋友的用户ID

            // 累加朋友购买的商品
            for (int j = 0; j < MAX_PRODUCTS; j++) {
                recommendation[j] += users[friendId].purchased[j];
            }
        }
    }

    // 排除用户自己已经购买的商品
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (users[userId].purchased[i] > 0) {
            recommendation[i] = 0; // 将已购买商品的推荐指数置为0
        }
    }

    // 按购买次数排序推荐商品
    int sortedIndices[MAX_PRODUCTS]; // 存储商品索引的数组
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        sortedIndices[i] = i;
    }

    for (int i = 0; i < MAX_PRODUCTS - 1; i++) {
        for (int j = 0; j < MAX_PRODUCTS - i - 1; j++) {
            if (recommendation[sortedIndices[j]] < recommendation[sortedIndices[j + 1]]) {
                int temp = sortedIndices[j];
                sortedIndices[j] = sortedIndices[j + 1];
                sortedIndices[j + 1] = temp;
            }
        }
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
    FILE *file = fopen("data.txt", "w");
    if (!file) {
        printf("无法打开文件进行保存！\n");
        return;
    }

    // 保存用户信息
    fprintf(file, "%d\n", userCount);
    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s %s\n", users[i].username, users[i].password);
        for (int j = 0; j < MAX_USERS; j++) {
            fprintf(file, "%d ", users[i].friends[j]);
        }
        fprintf(file, "\n");
        for (int j = 0; j < MAX_PRODUCTS; j++) {
            fprintf(file, "%d ", users[i].purchased[j]);
        }
        fprintf(file, "\n");
    }

    // 保存商品信息
    fprintf(file, "%d\n", productCount);
    for (int i = 0; i < productCount; i++) {
        fprintf(file, "%s %d\n", products[i].name, products[i].purchaseCount);
    }

    fclose(file);
    printf("数据已成功保存到文件！\n");
}

// 添加文件加载功能
void loadFromFile() {
    FILE *file = fopen("data.txt", "r");
    if (!file) {
        printf("数据文件不存在，程序将以空数据启动！\n");
        return;
    }

    // 加载用户信息
    fscanf(file, "%d\n", &userCount);
    for (int i = 0; i < userCount; i++) {
        fscanf(file, "%s %s\n", users[i].username, users[i].password);
        for (int j = 0; j < MAX_USERS; j++) {
            fscanf(file, "%d ", &users[i].friends[j]);
        }
        for (int j = 0; j < MAX_PRODUCTS; j++) {
            fscanf(file, "%d ", &users[i].purchased[j]);
        }
    }

    // 加载商品信息
    fscanf(file, "%d\n", &productCount);
    for (int i = 0; i < productCount; i++) {
        fscanf(file, "%s %d\n", products[i].name, &products[i].purchaseCount);
    }

    fclose(file);
    printf("数据已成功从文件加载！\n");
}

// 美化菜单界面，增加更清晰的提示信息
void displayMenu() {
    printf("\n========================================\n");
    printf("          朋友圈商品推荐系统           \n");
    printf("========================================\n");
    printf("1. 用户注册\n");
    printf("2. 用户登录\n");
    printf("3. 推荐商品\n"); // 新增推荐商品选项
    printf("4. 管理员功能\n");
    printf("5. 管理朋友圈\n");
    printf("6. 保存数据\n");
    printf("7. 退出系统\n");
    printf("========================================\n");
    printf("请选择操作（输入对应数字）：");
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
}

// 显示当前登录用户的购买数据
void showCurrentUserPurchases() {
    if (currentUserIndex == -1) {
        printf("请先登录！\n");
        return;
    }

    printf("当前用户：%s\n", users[currentUserIndex].username);
    printf("购买的商品：\n");
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (users[currentUserIndex].purchased[i] > 0) {
            printf("商品ID %d (数量 %d)\n", i, users[currentUserIndex].purchased[i]);
        }
    }
}

// 修改当前登录用户的购买数据
void modifyCurrentUserPurchases() {
    if (currentUserIndex == -1) {
        printf("请先登录！\n");
        return;
    }

    int productId, quantity;
    printf("请输入要修改的商品ID：");
    scanf("%d", &productId);

    if (productId < 0 || productId >= MAX_PRODUCTS) {
        printf("无效的商品ID！\n");
        return;
    }

    printf("请输入购买数量（输入0表示删除购买记录）：");
    scanf("%d", &quantity);

    if (quantity < 0) {
        printf("购买数量不能为负数！\n");
        return;
    }

    users[currentUserIndex].purchased[productId] = quantity;
    printf("购买数据已更新！\n");
}

// 添加好友
void addFriend() {
    if (currentUserIndex == -1) {
        printf("请先登录！\n");
        return;
    }

    char friendUsername[50];
    printf("请输入要添加的好友用户名：");
    scanf("%s", friendUsername);

    int friendIndex = findUserIndex(friendUsername);
    if (friendIndex == -1) {
        printf("未找到该用户！\n");
        return;
    }

    if (users[currentUserIndex].friends[friendIndex]) {
        printf("该用户已经是您的好友！\n");
        return;
    }

    users[currentUserIndex].friends[friendIndex] = 1;
    printf("好友添加成功！\n");
}

// 删除好友
void removeFriend() {
    if (currentUserIndex == -1) {
        printf("请先登录！\n");
        return;
    }

    char friendUsername[50];
    printf("请输入要删除的好友用户名：");
    scanf("%s", friendUsername);

    int friendIndex = findUserIndex(friendUsername);
    if (friendIndex == -1) {
        printf("未找到该用户！\n");
        return;
    }

    if (!users[currentUserIndex].friends[friendIndex]) {
        printf("该用户不是您的好友！\n");
        return;
    }

    users[currentUserIndex].friends[friendIndex] = 0;
    printf("好友删除成功！\n");
}

// 查询好友
void queryFriends() {
    if (currentUserIndex == -1) {
        printf("请先登录！\n");
        return;
    }

    printf("您的好友列表：\n");
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[currentUserIndex].friends[i]) {
            printf("好友用户名：%s\n", users[i].username);
        }
    }
}

int main() {
    int choice;
    loadFromFile(); // 启动时加载数据

    while (1) {
        displayMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 3: { // 推荐商品功能
                if (currentUserIndex == -1) {
                    printf("请先登录！\n");
                } else {
                    recommendProducts(currentUserIndex);
                }
                break;
            }
            case 4: { // 管理员功能
                int adminChoice;
                while (1) {
                    displayAdminMenu();
                    scanf("%d", &adminChoice);

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
                            goto mainMenu; // 返回主菜单
                        default:
                            printf("无效的选择，请重新输入！\n");
                    }
                }
                break;
            }
            case 5: { // 管理朋友圈
                int friendChoice;
                while (1) {
                    displayFriendMenu();
                    scanf("%d", &friendChoice);

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
                            goto mainMenu; // 返回主菜单
                        default:
                            printf("无效的选择，请重新输入！\n");
                    }
                }
                break;
            }
            case 6:
                saveToFile();
                break;
            case 7:
                saveToFile();
                printf("退出系统！\n");
                return 0;
            default:
                printf("无效的选择，请重新输入！\n");
        }

        mainMenu:; // 主菜单标签
    }
}