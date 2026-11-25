#include <stdio.h>
#include <stdlib.h>

typedef struct hmf_TreeNode {
    char data;
    int rank;
    struct hmf_TreeNode* left;
    struct hmf_TreeNode* right;
}hmf_TreeNode, *hmf_Tree;

static int cmp(const void* a, const void* b)
{
    const hmf_TreeNode* nodeA = *(hmf_TreeNode* const*)a;
    const hmf_TreeNode* nodeB = *(hmf_TreeNode* const*)b;
    if (nodeA->rank < nodeB->rank)
        return -1;
    else if (nodeA->rank > nodeB->rank)
        return 1;
    else
        return 0;
}

static hmf_TreeNode* CreateNode(char data, int rank, hmf_TreeNode* left, hmf_TreeNode* right)
{
    hmf_TreeNode* p = (hmf_TreeNode*)malloc(sizeof(hmf_TreeNode));
    if (p == NULL)
    {
        printf("内存分配失败！\n");
        exit(1);
    }
    p->data = data;
    p->rank = rank;
    p->left = left;
    p->right = right;
    return p;
}

static void Print_hfm_Tree(const hmf_TreeNode* root, int level)
{
    if (root == NULL)
        return;

    for (int i = 0; i < level; i++)
        printf("  ");
    if (root->left == NULL && root->right == NULL)
        printf("L %c %d\n", root->data, root->rank);
    else
        printf("I %d\n", root->rank);

    Print_hfm_Tree(root->left, level + 1);
    Print_hfm_Tree(root->right, level + 1);
}

static void Free_hfm_Tree(hmf_TreeNode* root)
{
    if (root == NULL)
        return;
    Free_hfm_Tree(root->left);
    Free_hfm_Tree(root->right);
    free(root);
}

static void Save_hfm_Tree(FILE* fp, const hmf_TreeNode* root)
{
    if (root == NULL)
        return;

    if (root->left == NULL && root->right == NULL)
        fprintf(fp, "L %c %d\n", root->data, root->rank);
    else
        fprintf(fp, "I %d\n", root->rank);

    Save_hfm_Tree(fp, root->left);
    Save_hfm_Tree(fp, root->right);
}

static void Read_hfm_Tree(FILE* fp, hmf_TreeNode** root)
{
    char type;
    if (fscanf(fp, " %c", &type) != 1)
        return;

    if (type == 'L')
    {
        char data;
        int rank;
        if (fscanf(fp, " %c %d", &data, &rank) != 2)
            return;
        *root = CreateNode(data, rank, NULL, NULL);
    }
    else if (type == 'I')
    {
        int rank;
        if (fscanf(fp, " %d", &rank) != 1)
            return;
        *root = CreateNode('\0', rank, NULL, NULL);
        Read_hfm_Tree(fp, &((*root)->left));
        Read_hfm_Tree(fp, &((*root)->right));
    }
}

static hmf_TreeNode* Init_hfm_Tree()
{
    int n;
    printf("请输入字符集大小 n: ");
    scanf("%d", &n);
    hmf_TreeNode** temp = (hmf_TreeNode**)malloc(sizeof(hmf_TreeNode*) * n);
    for(int i = 0; i < n; i++)
    {
        temp[i] = CreateNode('\0', 0, NULL, NULL);
        scanf(" %c", &temp[i]->data);
        scanf("%d", &temp[i]->rank);
    }

    int size = n;
    while(size > 1)
    {
        qsort(temp, size, sizeof(hmf_TreeNode*), cmp);
        hmf_TreeNode* parent = CreateNode('\0', temp[0]->rank + temp[1]->rank, temp[0], temp[1]);
        temp[0] = parent;
        temp[1] = temp[--size];   /* 先自减，避免读到 temp[n] 越界 */
    }

    hmf_TreeNode* root = temp[0];
    free(temp);

    FILE* fp = fopen("hfmtree", "w");
    if(fp == NULL)
    {
        printf("文件打开失败！\n");
        Free_hfm_Tree(root);
        exit(1);
    }
    fprintf(fp, "%d\n", n);
    Save_hfm_Tree(fp, root);
    fclose(fp);

    printf("\n哈夫曼树建立成功，结构如下（缩进表示层次）：\n");
    Print_hfm_Tree(root, 0);
    printf("\n已保存到文件 hfmtree。\n");
    return root;
}

/* 在哈夫曼树中查找字符 ch 的编码：
   找到返回 1，并把 0/1 串写入 path（以 '\0' 结尾）；
   找不到返回 0。左分支记 '0'，右分支记 '1'。 */
static int Encode_char(const hmf_TreeNode* root, char ch, char* path, int depth)
{
    if (root == NULL)
        return 0;

    /* 叶子结点：比较字符 */
    if (root->left == NULL && root->right == NULL)
    {
        if (root->data == ch)
        {
            path[depth] = '\0';
            return 1;
        }
        return 0;
    }

    path[depth] = '0';
    if (Encode_char(root->left, ch, path, depth + 1))
        return 1;

    path[depth] = '1';
    if (Encode_char(root->right, ch, path, depth + 1))
        return 1;

    return 0;
}

/* 编码：用已建好的哈夫曼树对文件 tobetrans 中的正文编码，
   结果写入文件 codefile。
   - root 不为空时直接用内存中的树；
   - root 为空时从文件 hfmtree 中读入树。 */
void Code_text(hmf_TreeNode* root)
{
    int ownTree = 0;          /* 树是否由本函数从文件读入（结束时需要释放） */
    FILE* fp_tree = NULL;

    if (root == NULL)
    {
        fp_tree = fopen("hfmtree", "r");
        if (fp_tree == NULL)
        {
            printf("文件 hfmtree 打开失败，请先执行初始化！\n");
            return;
        }
        int n = 0;
        if (fscanf(fp_tree, "%d", &n) != 1)   /* 跳过文件头部的 n */
        {
            printf("文件 hfmtree 内容错误！\n");
            fclose(fp_tree);
            return;
        }
        Read_hfm_Tree(fp_tree, &root);
        fclose(fp_tree);

        if (root == NULL)
        {
            printf("从文件 hfmtree 读取哈夫曼树失败！\n");
            return;
        }
        ownTree = 1;
    }

    FILE* fp_text = fopen("tobetrans", "r");
    if (fp_text == NULL)
    {
        printf("文件 tobetrans 打开失败！\n");
        if (ownTree)
            Free_hfm_Tree(root);
        return;
    }

    FILE* fp_code = fopen("codefile", "w");
    if (fp_code == NULL)
    {
        printf("文件 codefile 打开失败！\n");
        fclose(fp_text);
        if (ownTree)
            Free_hfm_Tree(root);
        return;
    }

    char path[256];
    int ch;
    int count = 0;   /* 成功编码的字符数 */
    int miss  = 0;   /* 不在字符集中的字符数 */

    while ((ch = fgetc(fp_text)) != EOF)
    {
        if (Encode_char(root, (char)ch, path, 0))
        {
            fputs(path, fp_code);
            count++;
        }
        else if (ch != '\n' && ch != '\r')   /* 回车换行不算错误，直接忽略 */
        {
            printf("警告：字符 '%c' 不在字符集中，已跳过。\n", ch);
            miss++;
        }
    }

    fclose(fp_text);
    fclose(fp_code);
    if (ownTree)
        Free_hfm_Tree(root);   /* 只释放本函数读入的树，不动调用者的树 */

    printf("编码完成：共编码 %d 个字符", count);
    if (miss > 0)
        printf("，%d 个字符因不在字符集中被跳过", miss);
    printf("，结果已写入文件 codefile。\n");
}

void Translate_Text(hmf_TreeNode* root)
{
    int ownTree = 0;          /* 树是否由本函数从文件读入（结束时需要释放） */
    FILE* fp_tree = NULL;

    if (root == NULL)
    {
        fp_tree = fopen("hfmtree", "r");
        if (fp_tree == NULL)
        {
            printf("文件 hfmtree 打开失败，请先执行初始化！\n");
            return;
        }
        int n = 0;
        if (fscanf(fp_tree, "%d", &n) != 1)   /* 跳过文件头部的 n */
        {
            printf("文件 hfmtree 内容错误！\n");
            fclose(fp_tree);
            return;
        }
        Read_hfm_Tree(fp_tree, &root);
        fclose(fp_tree);

        if (root == NULL)
        {
            printf("从文件 hfmtree 读取哈夫曼树失败！\n");
            return;
        }
        ownTree = 1;
    }

    FILE* fp_code = fopen("codefile", "r");
    if (fp_code == NULL)
    {
        printf("文件 codefile 打开失败！\n");
        if (ownTree)
            Free_hfm_Tree(root);
        return;
    }

    FILE* fp_text = fopen("decoded.txt", "w");
    if (fp_text == NULL)
    {
        printf("文件 decoded.txt 打开失败！\n");
        fclose(fp_code);
        if (ownTree)
            Free_hfm_Tree(root);
        return;
    }

    hmf_TreeNode* current = root;
    int ch;
    int count = 0;   /* 成功译码的字符数 */
    int miss  = 0;   /* 不在字符集中的编码数 */
    while((ch = fgetc(fp_code)) != EOF)
    {
        if (ch == '0')
            current = current->left;
        else if (ch == '1')
            current = current->right;
        else
        {
            printf("警告：编码文件中出现非法字符 '%c'，已跳过。\n", ch);
            miss++;
            continue;
        }

        if (current == NULL)
        {
            printf("警告：编码文件中出现无效路径，已跳过。\n");
            miss++;
            current = root;  /* 重置到根节点 */
            continue;
        }

        if (current->left == NULL && current->right == NULL)  /* 到达叶子结点 */
        {
            fputs(&current->data, fp_text);
            count++;
            current = root;  /* 重置到根节点 */
        }
    }
    fclose(fp_code);
    if (ownTree)
        Free_hfm_Tree(root);
}

/* (4) P：打印代码文件。
   把 codefile 以紧凑格式（每行 50 个代码）显示在终端，
   同时把字符形式的编码文件写入 codeprint。 */
void Print_Code(void)
{
    FILE* fp_code = fopen("codefile", "r");
    if (fp_code == NULL)
    {
        printf("文件 codefile 打开失败，请先执行编码！\n");
        return;
    }

    FILE* fp_print = fopen("codeprint", "w");
    if (fp_print == NULL)
    {
        printf("文件 codeprint 打开失败！\n");
        fclose(fp_code);
        return;
    }

    const int perLine = 50;
    int ch;
    int col = 0;      /* 当前行已输出的代码个数 */
    long total = 0;   /* 代码总数 */

    printf("\n代码文件 codefile 内容（每行 %d 个代码）：\n", perLine);
    while ((ch = fgetc(fp_code)) != EOF)
    {
        if (ch != '0' && ch != '1')   /* 跳过换行、空白等非代码字符 */
            continue;

        putchar(ch);
        fputc(ch, fp_print);
        total++;

        if (++col == perLine)         /* 满 50 个就换行，终端和文件同步 */
        {
            putchar('\n');
            fputc('\n', fp_print);
            col = 0;
        }
    }

    if (col != 0)   /* 末行不足 50 个，也要补一个换行 */
    {
        putchar('\n');
        fputc('\n', fp_print);
    }

    fclose(fp_code);
    fclose(fp_print);

    if (total == 0)
        printf("codefile 中没有代码。\n");
    else
        printf("共 %ld 个代码，已写入文件 codeprint。\n", total);
}

/* 递归输出哈夫曼树的一个结点的分支连线：
   prefix 是祖先已输出的缩进前缀，isLast 表示它是否为父结点的最后一个孩子。 */
static void Write_hfm_Tree_Branch(FILE* fp, const hmf_TreeNode* root,
                                  const char* prefix, int isLast, int isRoot)
{
    if (root == NULL)
        return;

    if (!isRoot)
        fprintf(fp, "%s%s", prefix, isLast ? "`-- " : "|-- ");

    if (root->left == NULL && root->right == NULL)
        fprintf(fp, "L %c %d\n", root->data, root->rank);
    else
        fprintf(fp, "I %d\n", root->rank);

    char childPrefix[2048];
    if (isRoot)
        childPrefix[0] = '\0';
    else
        snprintf(childPrefix, sizeof(childPrefix), "%s%s",
                 prefix, isLast ? "    " : "|   ");

    int hasLeft  = (root->left != NULL);
    int hasRight = (root->right != NULL);
    if (hasLeft)
        Write_hfm_Tree_Branch(fp, root->left, childPrefix, !hasRight, 0);
    if (hasRight)
        Write_hfm_Tree_Branch(fp, root->right, childPrefix, 1, 0);
}

/* (5) T：打印哈夫曼树。
   把内存中的哈夫曼树以直观的分支形式显示在屏幕上，
   同时把字符形式的哈夫曼树写入 treeprint。
   - root 不为空时直接用内存中的树；
   - root 为空时从文件 hfmtree 中读入树。 */
void Print_Tree(hmf_TreeNode* root)
{
    int ownTree = 0;          /* 树是否由本函数从文件读入（结束时需要释放） */

    if (root == NULL)
    {
        FILE* fp_tree = fopen("hfmtree", "r");
        if (fp_tree == NULL)
        {
            printf("文件 hfmtree 打开失败，请先执行初始化！\n");
            return;
        }
        int n = 0;
        if (fscanf(fp_tree, "%d", &n) != 1)   /* 跳过文件头部的 n */
        {
            printf("文件 hfmtree 内容错误！\n");
            fclose(fp_tree);
            return;
        }
        Read_hfm_Tree(fp_tree, &root);
        fclose(fp_tree);

        if (root == NULL)
        {
            printf("从文件 hfmtree 读取哈夫曼树失败！\n");
            return;
        }
        ownTree = 1;
    }

    FILE* fp_print = fopen("treeprint", "w");
    if (fp_print == NULL)
    {
        printf("文件 treeprint 打开失败！\n");
        if (ownTree)
            Free_hfm_Tree(root);
        return;
    }

    printf("\n哈夫曼树（L 叶子:字符 权值，I 内部结点:权值和）：\n");
    Write_hfm_Tree_Branch(stdout, root, "", 0, 1);
    Write_hfm_Tree_Branch(fp_print, root, "", 0, 1);

    fclose(fp_print);
    if (ownTree)
        Free_hfm_Tree(root);

    printf("\n已写入文件 treeprint。\n");
}

int main()
{
    hmf_Tree root = Init_hfm_Tree();
    if (root == NULL)
        return 1;

    Code_text(root);   /* 用内存中的树对 tobetrans 编码，写入 codefile */
    Translate_Text(root);  /* 用内存中的树对 codefile 译码，输出到屏幕 */

    Print_Code();      /* (4) P：显示 codefile 并写入 codeprint */
    Print_Tree(root);  /* (5) T：显示哈夫曼树并写入 treeprint */

    Free_hfm_Tree(root);
    return 0;
}