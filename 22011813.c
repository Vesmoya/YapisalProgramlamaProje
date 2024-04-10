#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define MAX_USERS 100

typedef struct
{
    char name[20];
    char lastName[20];
    char userName[20];
    char password[20];
    int maxScore;
} USER;

void clearInputBuffer() // fgets fonksiyonunu kullanırken ilk inputu \n olarak algılayıp bizden input girmemizi beklemiyor.
                        // Bu fonksiyonlar input buffer ı temizliyoruz.
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

char **memoryAlloc(int N, int M) //Lab matrisi için dinamik olarak hafıza ayırıyoruz.
{
    char **Lab;
    int i;
    Lab = (char **)malloc(N * sizeof(char *));
    if (Lab == NULL)
    {
        printf("Hafiza ayrilamadi!");
        exit(-1);
    }
    for (i = 0; i < N; i++)
    {
        Lab[i] = (char *)malloc(M * sizeof(char));
        if (Lab[i] == NULL)
        {
            printf("Hafiza ayrilamadi!...");
            exit(-1);
        }
    }

    return Lab;
}

void freeMemory(char **Lab, int N, int M) //hafızayı temizliyoruz.
{
    int i;
    for (i = 0; i < N; i++)
    {
        free(Lab[i]);
    }
    free(Lab);
}

int usernameControl(const char *username) //Girilen kullanıcı adı var mı diye kontrol ediyoruz.
{
    FILE *fp = fopen("users.txt", "rb");
    if (fp == NULL)
    {
        printf("Dosya acilamadi");
        exit(-1);
    }

    USER existingUser;

    while (fread(&existingUser, sizeof(USER), 1, fp) == 1)
    {
        if (strcmp(existingUser.userName, username) == 0)
        {
            fclose(fp);
            return 1; // kullanıcı adı mevcut
        }
    }

    fclose(fp);
    return 0; // kullanıcı adı mevcut değil
}

void register_User() //Kullanıcıyı dosyaya kayıt etme işlemleri.
{
    USER newUser;
    FILE *fp = fopen("users.txt", "ab"); //Dosyamızı binary formattı append modunda açıyoruz.
    if (fp == NULL)
    {
        perror("Dosya acilamadi");
        exit(-1);
    }
    printf("Ad: ");
    fgets(newUser.name, sizeof(newUser.name), stdin);
    newUser.name[strcspn(newUser.name, "\n")] = '\0'; // fgets input sonunda otomatik '\0' koyabiliyor bunu önlemek için fgetsden sonra kontrol yapıyoruz.
    printf("Soyad: ");
    fgets(newUser.lastName, sizeof(newUser.lastName), stdin);
    newUser.lastName[strcspn(newUser.lastName, "\n")] = '\0';
    do
    {
        printf("Kullanici Adi: ");
        fgets(newUser.userName, sizeof(newUser.userName), stdin);
        newUser.userName[strcspn(newUser.userName, "\n")] = '\0';

        if (usernameControl(newUser.userName))
        {
            printf("Bu kullanici adi zaten var. Lutfen baska bir kullanici adi secin.\n");
        }

    } while (usernameControl(newUser.userName)); // Kullanıcı adı varsa döngü devam ediyor farklı kullanıcı adı girmemizi istiyor.
    printf("Sifre: ");
    fgets(newUser.password, sizeof(newUser.password), stdin);
    newUser.password[strcspn(newUser.password, "\n")] = '\0';

    newUser.maxScore = 0; //Kullanıcı yeni oluşturulduğu için skoru 0 olarak atıyoruz.

    fwrite(&newUser, sizeof(USER), 1, fp); //Kullanıcıyı dosyaya kaydediyoruz
    fclose(fp);
    printf("Kayit basarili.\n");
}

USER login_User()   //Login işlemleri.
{
    USER users[MAX_USERS];
    char enteredUserName[20];
    char enteredPassword[20];
    FILE *fp = fopen("users.txt", "rb"); //Dosyayı read ve binary modda açıyoruz.
    if (fp == NULL)
    {
        perror("Dosya acilamadi\n");
        exit(-1);
    }
    printf("Kullanici adi: ");
    fgets(enteredUserName, sizeof(enteredUserName), stdin);
    enteredUserName[strcspn(enteredUserName, "\n")] = '\0'; // Burada username kontrolleri sırasında sorun olmaması için '\0' varsa temizliyoruz.
    printf("Sifre: ");
    fgets(enteredPassword, sizeof(enteredPassword), stdin);
    enteredPassword[strcspn(enteredPassword, "\n")] = '\0';

    char line[100];
    USER tmpUser;

    while (fread(&tmpUser, sizeof(USER), 1, fp) == 1) //Geçici bir tmpUser oluşturup dosyadan okuduğumuz kullanıcı bilgilerini burada tutuyoruz.
    {
        //Altta da girilen giriş bilgilerini geçici olarak tuttuğumuz tmpUser ile karşılaştırıyoruz. Doğruysa kullanıcı o kişidir.
        if (strcmp(tmpUser.userName, enteredUserName) == 0 && strcmp(tmpUser.password, enteredPassword) == 0)
        {
            printf("Giris basarili!\n");

            fclose(fp);
            return tmpUser; // Giriş başarılı
        }
    }
    printf("Giris basarisiz.\nLutfen kullanici adi ve sifrenizi tekrar giriniz.\n");
    return login_User(); //Bilgiler hatalıysa recursive olarak bu fonksiyonu tekrar çağırıyoruz.
}
USER login_Screen() //Giriş yaptığımız ekran.
{
    int secim;
    USER currentUser;

    while (1)
    {
        printf("1 - Kayit Ol\n");
        printf("2 - Giris Yap\n");
        printf("Seciminizi yapin: ");
        scanf("%d", &secim);

        switch (secim)
        {
        case 1:
            clearInputBuffer();//Giriş bilgilerini alırken sorun olmaması için input buffer ı temizliyoruz.
            register_User();
            break;
        case 2:
            clearInputBuffer();
            return login_User();
        default:
            printf("Geçersiz secim!\n");
        }
    }
}
char **harita_yukle(int input, int N, int M, int autoMod)
{

    int i, j;
    char **Lab;
    char mapName[10];
    switch (input)
    {
    case 1:
        // hazır harita yükle

        // dinamik olarak Lab[N][M] matrisini oluşturalım.
        Lab = memoryAlloc(N, M);
        if (autoMod == 0) //Auto modda çalıştırılmadıysa harita ismini kullanıcıdan alıyoruz yoksa otomatik giriyoruz.
        {
            printf("Yukleyeceginiz haritanin dosya adini sonu .txt olacak sekilde giriniz:");
            scanf("%s", mapName);
        }
        else
            strcpy(mapName, "map1.txt");

        FILE *file = fopen(mapName, "r");
        if (file == NULL)
        {
            printf("Error opening file");
            exit(-1);
        }

        for (i = 0; i < N; i++)
        {
            for (j = 0; j < M; j++)
            {
                fscanf(file, " %c,", &Lab[i][j]);//Harita bilgilerini dosyadan okuyup Lab matrisine kaydediyoruz.
            }
        }

        fclose(file);
        return Lab; //Haritamızı kaydettiğimiz matrisimizi döndürüyoruz.

    case 2:
        // yeni harita oluştur

        // dinamik olarak Lab[N][M] matrisini oluşturalım.
        Lab = memoryAlloc(N, M);

        for (i = 0; i < N; i++)
        {
            // Haritamızı tutacağımız matrisimize harita değerlerini elle girip kendimiz harita oluşturuyoruz.
            printf("%d nolu satir degerlerini sirayla giriniz.\n", i);
            for (j = 0; j < M; j++)
            {
                scanf(" %c", &Lab[i][j]);
            }
        }
        return Lab;

    default:
        printf("Geçersiz secim!\n");
    }
}

void printMatrix(char **Lab, int N, int M) //Haritamızı ekrana yazdıran fonksiyon.
{
    int i,j;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            printf("%c ", Lab[i][j]);
        }
        printf("\n");
    }
}

void update_score(char madde, int *kp_say, int *ke_say) //Oyun sırasında skoru güncelleyen fonksiyon.
{
    if (madde == 'p')
    {
        (*kp_say)++;
    }
    else if (madde == 'P')
    {
        (*kp_say)--;
    }
    else if (madde == 'E')
    {
        (*ke_say)++;
    }
    else if (madde == 'e')
    {
        (*ke_say)--;
    }
}
//Haritadaki hareketleri yapmamızı sağlayan fonksiyon.
void lab_movement(int yon, char **Lab, int N, int M, int *kp_say, int *ke_say, int *cikis_control, int *karadelik_ctrl)
{
    int i = 0, j = 0, x, y;
    int found = 0;

    while (found == 0 && i < N)
    {
        j = 0;
        while (found == 0 && j < M)
        {
            if (Lab[i][j] == 'X')
            {
                x = i; // mevcut konum değerleri
                y = j;
                found = 1;
            }
            j++;
        }
        i++;
    }
    if (found == 0) //Kontrol edebileceğimiz bir X değeri yoksa program sonlanıyor.
    {
        printf("X degeri bulunamadi!\n");
        exit(-1);
    }

    switch (yon) //Gideceğimiz yöne göre hareket işlemleri
    {
    case 1: // yukarı
        if (x > 0 && Lab[x - 1][y] != '1')
        {
            if (Lab[x - 1][y] == 'C')
                *cikis_control = 1;
            if (Lab[x - 1][y] == 'K')
                *karadelik_ctrl = 1;
            update_score(Lab[x - 1][y], kp_say, ke_say);
            Lab[x][y] = '0';
            Lab[x - 1][y] = 'X';
        }
        break;
    case 2: // aşağı
        if (x < N - 1 && Lab[x + 1][y] != '1')
        {
            if (Lab[x + 1][y] == 'C')
                *cikis_control = 1;
            if (Lab[x + 1][y] == 'K')
                *karadelik_ctrl = 1;
            update_score(Lab[x + 1][y], kp_say, ke_say);
            Lab[x][y] = '0';
            Lab[x + 1][y] = 'X';
        }
        break;

    case 3: // sola
        if (y > 0 && Lab[x][y - 1] != '1')
        {
            if (Lab[x][y - 1] == 'C')
                *cikis_control = 1;
            if (Lab[x][y - 1] == 'K')
                *karadelik_ctrl = 1;
            update_score(Lab[x][y - 1], kp_say, ke_say);
            Lab[x][y] = '0';
            Lab[x][y - 1] = 'X';
        }
        break;
    case 4: // sağa
        if (y < M - 1 && Lab[x][y + 1] != '1')
        {
            if (Lab[x][y + 1] == 'C')
                *cikis_control = 1;
            if (Lab[x][y + 1] == 'K')
                *karadelik_ctrl = 1;
            update_score(Lab[x][y + 1], kp_say, ke_say);
            Lab[x][y] = '0';
            Lab[x][y + 1] = 'X';
        }
        break;
    }
}
// Kullanıcının aldığı skor önceki maxScore dan büyükse maxScore u güncelliyor.
void updateMaxScore(const char *username, int score)
{
    FILE *fp = fopen("users.txt", "r+b");
    if (fp == NULL)
    {
        perror("Dosya acilamadi");
        exit(-1);
    }

    USER currentUser;

    while (fread(&currentUser, sizeof(USER), 1, fp) == 1)
    {
        if (strcmp(currentUser.userName, username) == 0)
        {
            // Elde edilen skor daha yüksekse güncelle
            if (score > currentUser.maxScore)
            {
                currentUser.maxScore = score;
                fseek(fp, -sizeof(USER), SEEK_CUR);
                fwrite(&currentUser, sizeof(USER), 1, fp);
            }
            break;
        }
    }

    fclose(fp);
}
//En yüksek skorlu kullanıcıları listeleyen fonksiyon.
void listTopFive()
{
    int i, j;
    FILE *fp = fopen("users.txt", "rb");
    if (fp == NULL)
    {
        perror("Dosya acilamadi");
        exit(-1);
    }

    // Kullanıcıları bir diziye kaydedelim.
    USER users[MAX_USERS];
    int numUsers = 0;

    while (fread(&users[numUsers], sizeof(USER), 1, fp) == 1)
    {
        numUsers++;
    }

    fclose(fp);

    // Kullanıcıları skorlarına göre bubble sort ile büyükten küçüğe sıralayalım.
    for (i = 0; i < numUsers - 1; i++)
    {
        for (j = 0; j < numUsers - i - 1; j++)
        {
            if (users[j].maxScore < users[j + 1].maxScore)
            {
                USER temp = users[j];
                users[j] = users[j + 1];
                users[j + 1] = temp;
            }
        }
    }

    // En yüksek 5 kullanıcıyı yazdır.
    printf("\nEn yuksek skora sahip 5 kullanici:\n");
    for (i = 0; i < 5 && i < numUsers; i++)
    {
        printf("%d: %s %s - Score: %d\n", i + 1, users[i].name, users[i].lastName, users[i].maxScore);
    }
}

int main()
{
    int i = 0, j = 0, k, input = 0, N, M, mod;
    int ke_say = 0, kp_say = 0; // karşıt elektron ve karşıt proton sayacı
    char a, old[2];
    USER currentUser;
    char **Lab;
    int otoInputs[12] = {1, 4, 4, 2, 2, 4, 4, 1, 1, 2, 2, 2}; //autoMod da kullanacağımız input listesi.
    int autoPlay_check = 0;
    int cikis_kontrol = 0, karadelik_ctrl = 0;

    printf("Karsit madde labirenti oyununa hosgeldiniz.\nLabirentte x isareti sizi, c ise cikis hedefini gosterir.\n\
Amaciniz e+ ve p- maddelerini toplayarak oyun sonu karsit madde uretmektir.\n\
e- ve p+ maddeleri sayacinizi dusurur dikkat ediniz.\n\
k noktasi ise kara deliktir, dokunursaniz oyununuz basarisiz olur.\nBasarilar!\n");

    currentUser = login_Screen();//Login ekranı
    printf("En yuksek skorunuz:%d\n", currentUser.maxScore);
    listTopFive();

    printf("Auto Play modu icin 1'e normal mod icin 2'ye basiniz:");//Mod seçimi
    scanf("%d", &mod);

    switch (mod)
    {
    case 1:
        // autoPlay için otomatik harita yükleme.
        N = 6;
        M = 10;
        Lab = harita_yukle(1, N, M, 1);
        autoPlay_check = 1;
        break;
    case 2:
        while (input != 1 && input != 2)
        {
            printf("Hazir harita kullanmak icin 1'i, harita yuklemek icin 2'yi tuslayiniz: ");
            scanf("%d", &input);
            if (input != 1 && input != 2)
                printf("Hatali tuslama yaptiniz.\n");
            else
            {
                printf("harita boyutlarini giriniz.\nHazir harita sectiyseniz lutfen dosyadaki haritanin matris boyutlarini giriniz.\n");
                printf("Satir sayisi:");
                scanf("%d", &N);
                printf("Sutun sayisi:");
                scanf("%d", &M);
                Lab = harita_yukle(input, N, M, 0);
            }
        }
        break;
    default:
        printf("hatali secim yaptiniz.\n");
    }

    system("cls");

    int found = 0;

    //Haritadaki G değerini bulup kullanıcı olarak kullanacağımız X ile değiştiriyoruz.
    while (found == 0 && i < N)
    {
        j = 0;
        while (found == 0 && j < M)
        {
            if (Lab[i][j] == 'G')
            {
                Lab[i][j] = 'X';
                found = 1;
            }
            j++;
        }
        i++;
    }

    //autoPlay modunda oynuyorsak if işlemleri yoksa else işlemleri yapılıyor.
    if (autoPlay_check == 1)
    {
        j = 0;
        for (i = 20; i > 0; i--)
        {
            //Her harekette süreyi, madde sayılarını, ve matrisi ekrana yazdırıyoruz.
            printf("Geri Sayim:%d\n", i);
            printf("Karsit proton sayiniz:%d\n", kp_say);
            printf("Karsit elektron sayiniz:%d\n", ke_say);
            printMatrix(Lab, N, M);

            a = otoInputs[j];
            lab_movement(a, Lab, N, M, &kp_say, &ke_say, &cikis_kontrol, &karadelik_ctrl);
            j++;
            sleep(1);
            system("cls");
            if (cikis_kontrol == 1)
            {
                printf("Cikisa ulastiniz.\n");
                break;
            }
        }
    }
    else
    {
        for (i = 20; i > 0; i--)
        {
            // Her harekette süreyi, madde sayılarını, ve matrisi ekrana yazdırıyoruz.
            printf("Geri Sayim:%d\n", i);
            printf("Karsit proton sayiniz:%d\n", kp_say);
            printf("Karsit elektron sayiniz:%d\n", ke_say);
            printMatrix(Lab, N, M);

            //Esc ile oyundan çıkışı kontrol ediyoruz.
            a = getch();
            if (a == 27)
            {
                printf("Cikis\n");
                break;
            }
            else
            {
                a = getch();
                switch (a)
                {
                case 72: // Yukarı ok tuşu
                    printf("Yukariya hareket et\n");
                    lab_movement(1, Lab, N, M, &kp_say, &ke_say, &cikis_kontrol, &karadelik_ctrl);
                    break;
                case 80: // Aşağı ok tuşu
                    printf("Asagiya hareket et\n");
                    lab_movement(2, Lab, N, M, &kp_say, &ke_say, &cikis_kontrol, &karadelik_ctrl);
                    break;
                case 75: // Sol ok tuşu
                    printf("Sola hareket et\n");
                    lab_movement(3, Lab, N, M, &kp_say, &ke_say, &cikis_kontrol, &karadelik_ctrl);
                    break;
                case 77: // Sağ ok tuşu
                    printf("Saga hareket et\n");
                    lab_movement(4, Lab, N, M, &kp_say, &ke_say, &cikis_kontrol, &karadelik_ctrl);
                    break;
                default:
                    printf("Bilinmeyen tus: %d\n", a);
                }
            }
            sleep(1);
            system("cls");
            //Hareket sonrası ulaştığımız yer çıkış ve ya kara delikse döngüden çıkıyoruz.
            if (cikis_kontrol == 1)
            {
                printf("Cikisa ulastiniz.\n");
                break;
            }
            if (karadelik_ctrl == 1)
            {
                printf("OOOPS! kara delige yakalandiniz.\n");
                break;
            }
        }
    }
    if (i == 0) //i 0 olduysa yani süre bittiyse
    {
        printf("BOOOOM!\n");
        printf("Zamaninda cikisa ulasamadiniz.\n");
    }

    if (ke_say > 0 && kp_say > 0) //2 değerde 0 dan büyükken küçük değer kadar karşıt madde üretilmiştir.
    {
        if (ke_say >= kp_say)
        {
            printf("Uretilen toplam karsit madde sayisi ve skorunuz:%d\n", kp_say);
            updateMaxScore(currentUser.userName, kp_say);
        }
        else
        {
            printf("Uretilen toplam karsit madde sayisi ve skorunuz:%d\n", ke_say);
            updateMaxScore(currentUser.userName, ke_say);
        }
    }
    else
        printf("Skorunuz:0\n");
    freeMemory(Lab, N, M);
    system("PAUSE");
    return 0;
}
