# Laporan Resmi Sistem Operasi Modul4 E09

#### Fandi Pranata Jaya - 05111740000056
#### Fadhil Musaad Al Giffary - 05111740000116

## Nomor 1
### Soal:
1. Semua nama file dan folder harus terenkripsi
<br/>Enkripsi yang Atta inginkan sangat sederhana, yaitu Caesar cipher. Namun, Kusuma mengatakan enkripsi tersebut sangat mudah dipecahkan. Dia menyarankan untuk character list diekspansi,tidak hanya alfabet, dan diacak. Berikut character list yang dipakai:
<br/><br/>qE1~ YMUR2"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\8s;g<{3.u*W-0
<br/><br/>Misalkan ada file bernama “halo” di dalam folder “INI_FOLDER”, dan key yang dipakai adalah 17, maka:
<br/>“INI_FOLDER/halo” saat belum di-mount maka akan bernama “n,nsbZ]wio/QBE#”, saat telah di-mount maka akan otomatis terdekripsi kembali menjadi “INI_FOLDER/halo” .
<br/>Perhatian: Karakter ‘/’ adalah karakter ilegal dalam penamaan file atau folder dalam *NIX, maka dari itu dapat diabaikan

### Jawaban:
Pertama - tama masukkan character list untuk ciphernya kedalam array of char, kemudian masukkan juga key yang dipakai. Kemudian buat fungsi enkripsi dan dekripsinya. Syntaxnya seperti berikut ini:
```
#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
char cipher[] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
int key;

char enkripsi(char *nama)
{
	char *tmp;
	for(int i=0; i<strlen(nama); i++){
		tmp=strchr(cipher,nama[i]);
		if(tmp){
			int index = tmp - cipher;
			nama[i]=cipher[(index+key)%strlen(cipher)];
		}
	}
	return *nama;
}

char dekripsi(char *nama)
{
	char *tmp;
	for(int i=0; i<strlen(nama); i++){
		tmp=strchr(cipher,nama[i]);
		if(tmp){
			int index = tmp - cipher;
			index = index-key;
			if(index<0)index+=strlen(cipher);
			nama[i]=cipher[index];
		}
	}
	return *nama;
}

int main(int argc, char *argv[])
{
	scanf("Key: %d",key);
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
```
Kemudian untuk setiap fungsi fuse yang ada, path awalnya dienkripsi karena mountpoint fusenya dalam bentuk dekripsi dari folder `/home/[user]/shift4` yang sudah terenkripsi sebelum di mount. Kemudian khusus untuk fungsi `xmp_readdir()` sebelum di `filler()`, maka path harus didekripsi terlebih dahulu agar bisa dibaca difolder fusenya.

## Nomor 2
### Soal:
2. Semua file video yang tersimpan secara terpecah-pecah (splitted) harus secara otomatis tergabung (joined) dan diletakkan dalam folder “Videos”
<br/>Urutan operasi dari kebutuhan ini adalah:
<br/>a. Tepat saat sebelum file system di-mount
	<br/>i. Secara otomatis folder “Videos” terbuat di root directory file system
	<br/>ii. Misal ada sekumpulan file pecahan video bernama “computer.mkv.000”, “computer.mkv.001”, “computer.mkv.002”, dst. Maka secara otomatis file pecahan tersebut akan di-join menjadi file video “computer.mkv”
Untuk mempermudah kalian, dipastikan hanya video file saja yang terpecah menjadi beberapa file. File pecahan tersebut dijamin terletak di root folder fuse
	<br/>iii. Karena mungkin file video sangat banyak sehingga mungkin saja saat menggabungkan file video, file system akan membutuhkan waktu yang lama untuk sukses ter-mount. Maka pastikan saat akan menggabungkan file pecahan video, file system akan membuat 1 thread/proses(fork) baru yang dikhususkan untuk menggabungkan file video tersebut
	<br/>iv. Pindahkan seluruh file video yang sudah ter-join ke dalam folder “Videos”
	<br/>v. Jangan tampilkan file pecahan di direktori manapun
<br/>b. Tepat saat file system akan di-unmount
<br/>i. Hapus semua file video yang berada di folder “Videos”, tapi jangan hapus file pecahan yang terdapat di root directory file system
<br/>ii. Hapus folder “Videos” 
### Jawaban:
Dalam soal ini kita membutuhkan fungsi `xmp_init()` yaitu fungsi yang dijalankan sebelum fuse di mount dan fungsi `xmp_destroy()` yaitu fungsi yang dijalankan sebelum fuse di unmount dan thread `merge()` untuk menggabungkan video kedalam folder `/Video`.fungsi `xmp_init()` memanggil thread `merge()` dan fungsi `xmp_destroy()` berisi fungsi untuk mendestroy folder `/Video` dan semua file didalamnya. Syntax nya adalah seperti berikut ini:
```
void incr(char x[1005]) {
		printf("%s\n", x);
	if (x[3] != '9') {
		x[3] = x[3] + 1;
		return;
	}
	x[3] = '0';
	if (x[2] != '9') {
		x[2] = x[2] + 1;
		return;
	}
	x[2] = '0';
	x[1] = x[1] + 1;
	return;
}

void* merge(void *arg) {
	char root[1005] = "/home/fandipj/shift4";
	char *fpath = (char*)arg;
	printf("%s\n", (char*)arg);
	fpath[strlen(fpath) - 4] = '\0';
	char vid[1005] = "/Video";
	enkripsi(vid);
	strcat(root, vid);
	strcat(root, fpath + 20);
	dekripsi(root);
	printf("%s\n", root);
	enkripsi(root);
	FILE* lastfile = fopen(root, "a");
	strcat(vid, fpath);
	FILE *fp;
	char counter[1005] = ".000";
	char nowpath[1005];
	char encryptcounter[1005];
	strcpy(encryptcounter, counter);
	enkripsi(encryptcounter);
	strcpy(nowpath, fpath);
	strcat(nowpath, encryptcounter);
	printf("%s\n", nowpath);
	while ((fp = fopen(nowpath, "r"))) {
		char c;
		while ((c = fgetc(fp)) != EOF) 
		{ 
  		    fputc(c, lastfile); 
  		}
		fclose(fp);
		incr(counter);
		strcpy(encryptcounter, counter);
		enkripsi(encryptcounter);
		strcpy(nowpath, fpath);
		strcat(nowpath, encryptcounter);
		printf("%s\n", nowpath);
	}
	fclose(lastfile);
	return NULL;
}

void *xmp_init(struct fuse_conn_info *conn) {
	char root[1005] = "/home/fandipj/shift4";
	DIR *dp;
	struct dirent *de;
	char vid[1005];
	char tmpp[1005] = "/Video";
	enkripsi(tmpp);
	strcpy(vid, root);
	strcat(vid, tmpp);
	int err = mkdir(vid, 0750);
	printf("%s %d\n", vid, err);
	dp = opendir(root);
	int i = 0;
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		char tmppath[1005];
		strcpy(tmppath, root);
		strcat(tmppath, "/");
		strcpy(tmpp, de->d_name);
		strcat(tmppath, tmpp);
		stat(tmppath, &st);
		if (S_ISREG(st.st_mode) == 0)
			continue;
		int sz = strlen(de->d_name);
		char name[1005];
		strcpy(name, de->d_name);
		dekripsi(name);
		if (name[sz - 1] != '0' || name[sz - 2] != '0' || name[sz - 3] != '0'
			|| name[sz - 4] != '.')
			continue;
		printf("%s\n", tmppath);
		printf("%s\n", name);
		pthread_create(&(tid[0]), NULL, &merge, tmppath);
		pthread_join(tid[0],NULL);
		i++;
	}
	
	closedir(dp);

	return NULL;
}

void xmp_destroy(void* privateData) {
	char root[1005] = "/home/fandipj/shift4/g[xO#";
	DIR *dp;
	struct dirent *de;
	printf("%s\n", root);
	dp = opendir(root);
	while ((de = readdir(dp)) != NULL) {
		char path[1005];
		strcpy(path, root);
		strcat(path, "/");
		if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
			continue;
		strcat(path, de->d_name);
		printf("%s\n", path);
		remove(path);
	}
	closedir(dp);
	rmdir(root);
}
```

## Nomor 3
### Soal:
3. Sebelum diterapkannya file system ini, Atta pernah diserang oleh hacker LAPTOP_RUSAK yang menanamkan user bernama “chipset” dan “ic_controller” serta group “rusak” yang tidak bisa dihapus. Karena paranoid, Atta menerapkan aturan pada file system ini untuk menghapus “file bahaya” yang memiliki spesifikasi:
	* Owner Name     : ‘chipset’ atau ‘ic_controller’
	* Group Name    : ‘rusak’
	* Tidak dapat dibaca

Jika ditemukan file dengan spesifikasi tersebut ketika membuka direktori, Atta akan menyimpan nama file, group ID, owner ID, dan waktu terakhir diakses dalam file “filemiris.txt” (format waktu bebas, namun harus memiliki jam menit detik dan tanggal) lalu menghapus “file bahaya” tersebut untuk mencegah serangan lanjutan dari LAPTOP_RUSAK
### Jawaban:
Pertama - tama untuk mendapatkan status dari semua file yang ada dalam fuse, maka dalam fungsi `xmp_readdir()` dibuat `struct stat`. Syntaxnya seperti berikut ini:
```
while ((de = readdir(dp)) != NULL) {
	struct stat tmp;
	char tmppath[1005];
	strcpy(tmppath, fpath);
	strcat(tmppath, "/");
	strcat(tmppath, de->d_name);
	stat(tmppath, &tmp);
}
```
Kemudian untuk mendapatkan status dari user, maka digunakan `struct passwd` dan untuk mendapatkan status dari group, maka digunakan `struct group`. Kemudian dalam fungsi `xmp_readdir()` dilakukan pengecekan jika usernya bernama "chipset" atau "ic_controller" dan groupnya adalah "rusak", maka buat file baru dengan `FILE` dan mengambil time pada saat akses terakhir file yang usernya  bernama "chipset" atau "ic_controller" dan groupnya adalah "rusak", mengambil nama file, group ID, owner ID. Kemudian memasukkan informasi tersebut kedalam file "filemiris.txt". Kemudian, file yang usernya  bernama "chipset" atau "ic_controller" dan groupnya adalah "rusak" akan dihapus dengan fungsi c `remove()`. Syntaxnya adalah seperti berikut ini:
```
while ((de = readdir(dp)) != NULL) {
	if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
		continue;
	}
	struct stat st;
	memset(&st, 0, sizeof(st));
	st.st_ino = de->d_ino;
	st.st_mode = de->d_type << 12;
	struct stat tmp;
	char tmppath[1005];
	strcpy(tmppath, fpath);
	strcat(tmppath, "/");
	strcat(tmppath, de->d_name);
	stat(tmppath, &tmp);
	dekripsi(de->d_name);
	printf("NAMA FILE %s\n", de->d_name);
	struct passwd *pw = getpwuid(tmp.st_uid);
	struct group *gr = getgrgid(tmp.st_gid);
	printf("Name %s, Group %s\n", pw->pw_name, gr->gr_name);
	if ((strcmp(pw->pw_name, "chipset") == 0 || strcmp(pw->pw_name, "ic_controller") == 0) && strcmp(gr->gr_name, "rusak") == 0 &&  (tmp.st_mode & 4) == 0) {
		printf("Masuk Pak Eko\n");
		FILE* fptr;
		char accesstime[55];
		time_t t = tmp.st_atime;
		struct tm lt;
		localtime_r(&t, &lt);

		strftime(accesstime, 55, "%d.%m.%Y %H:%M:%S", &lt);
		char buffer[1005];
		memset(buffer, 0, sizeof(buffer));
		strcat(buffer, de->d_name);
		enkripsi(de->d_name);
		strcat(buffer, " ");
		char ttt[15];
		sprintf(ttt, "%d", gr->gr_gid);
		strcat(buffer, ttt);
		sprintf(ttt, "%d", pw->pw_uid);
		strcat(buffer, " ");
		strcat(buffer, ttt);
		strcat(buffer, " ");
		strcat(buffer, accesstime);
		char miris[1005] = "filemiris.txt";
		enkripsi(miris);
		char dest[1005];
		strcpy(dest, fpath);
		strcat(dest, "/");
		strcat(dest, miris);
		printf("%s\n", dest);
		fptr = fopen(dest, "a+");
		printf("%s\n", buffer);
		fprintf(fptr, "%s\n", buffer);
		fclose(fptr);
		printf("%s\n", de->d_name);
		strcpy(dest, fpath);
		strcat(dest, "/");
		strcat(dest, de->d_name);
		remove(dest);
		continue;
	}
}
```

## Nomor 4
### Soal:
4. Pada folder <b>YOUTUBER</b>, setiap membuat folder permission foldernya akan otomatis menjadi 750. Juga ketika membuat file permissionnya akan otomatis menjadi 640 dan ekstensi filenya akan bertambah “.iz1”. File berekstensi “.iz1” tidak bisa diubah permissionnya dan memunculkan error bertuliskan “File ekstensi iz1 tidak boleh diubah permissionnya.”
### Jawaban:
Pertama - tama untuk merubah permission ketika folder dibuat dalam folder `/YOUTUBER` didalam fungsi `xmp_mkdir()`sebelum path dienkripsi, maka dicek apakah path mengandung string `/YOUTUBER`, jika iya, maka mode dari folder yang akan dibuat dirubah menjadi mode 750. Syntaxnya seperti berikut ini:
```
static int xmp_mkdir(const char *path, mode_t mode)
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	if (strstr(path_temp, "/YOUTUBER") != NULL) {
		mode = 0000750;
	}
	
	enkripsi(path_temp);
	
	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
	}
    int res;

    res = mkdir(fpath, mode);
    if(res == -1)
        return -errno;

    return 0;
}
```
Kemudian untuk merubah permission ketika file dibuat dalam folder `/YOUTUBER` didalam fungsi `xmp_create()` sebelum path dienkripsi, maka dicek apakah path mengandung string `/YOUTUBER`, jika iya, maka mode dari file yang akan dibuat dirubah menjadi mode 640. Selain itu, agar bisa membuat file di fuse, maka perlu juga untuk mengimplementasikan fungsi `xmp_utimens()`. Syntaxnya seperti berikut ini:
```
static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	
	if (strstr(path_temp, "/YOUTUBER") != NULL) {
		mode = 0000640;
	}

	enkripsi(path_temp);
	
	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
	}

	memset(temp_path,0,sizeof(temp_path));
	strcpy(temp_path,path);
	
	int fd;
	
	fd = creat(fpath, mode);
	if (fd == -1)
		return -errno;
	close(fd);
	flag = 0;
	int err = pthread_create(&(tid[0]), NULL, &renamefile, fpath);
	if (err != 0)
		printf("\n ===***can't create thread : [%s]",strerror(err));
	else 
		printf("\n ===***create thread success\n");

	return 0;
}


static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	
	enkripsi(path_temp);
		
	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
	}

	int res;

	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, fpath, ts, AT_SYMLINK_NOFOLLOW);
	flag = 1;
	if (res == -1)
		return -errno;

	return 0;
}
```
Kemudian untuk menambah ekstensi ".iz1", maka dalam fungsi `xmp_create()` setelah file di close, maka akan dibuat thread untuk merename file yang sudah dibuat tadi menjadi berekstensi ".iz1" yang bernama `renamefile()`. Didalam fungsi tersebut digunakan fungsi c `rename()` untuk merename file dalam folder `/YOUTUBER`. Fungsi ini akan berjalan jika fungsi `xmp_utimens()` sudah selesai dijalankan. Syntaxnya adaklah seperti berikut ini:  
```
void* renamefile(void *arg)
{
	char *path_now = (char*)arg;
	pthread_t id = pthread_self();
	if (pthread_equal(id, tid[0])) {
		if (strstr(path_now, "/YOUTUBER") != NULL) {
			while (flag == 0) {}
			char newname[1005],oldname[1005];
			enkripsi(path_now);
			sprintf(oldname, "%s%s",dirpath,path_now);
			strcpy(newname, oldname);
			char ext[] = ".iz1";
			enkripsi(ext);
			strcat(newname, ext);
			rename(oldname, newname);
		}
	}
	return NULL;
}
```
Kemudian untuk menampilkan pesan error ketika file ".iz1" dalam folder `/YOUTUBER` di chmod, maka dalam fungsi `xmp_chmod()` dicek, jika file dalam folder `/YOUTUBER` di chmod, maka print “File ekstensi iz1 tidak boleh diubah permissionnya.” dan file tersebut tidak bisa di chmod (return -1). Syntaxnya seperti berikut ini:
```

static int xmp_chmod(const char *path, mode_t mode)
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);	
	
	int sz = strlen(path_temp);
	if (strstr(path_temp, "/YOUTUBER/") != NULL && path_temp[sz - 1] == '1' 
		&& path_temp[sz - 2] == 'z' && path_temp[sz - 3] == 'i' && path_temp[sz - 4] == '.') {
    	printf("File ekstensi iz1 tidak boleh diubah permissionnya.\n");
    	return -1;
	}

	enkripsi(path_temp);
	
	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
	}

	int res;

	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}
```
## Nomor 5
### Soal:
5. Ketika mengedit suatu file dan melakukan save, maka akan terbuat folder baru bernama Backup kemudian hasil dari save tersebut akan disimpan pada backup dengan nama namafile_[timestamp].ekstensi. Dan ketika file asli dihapus, maka akan dibuat folder bernama RecycleBin, kemudian file yang dihapus beserta semua backup dari file yang dihapus tersebut (jika ada) di zip dengan nama namafile_deleted_[timestamp].zip dan ditaruh ke dalam folder RecycleBin (file asli dan backup terhapus). Dengan format [timestamp] adalah yyyy-MM-dd_HH:mm:ss
### Jawaban:
Pertama - tama pada fungsi `xmp_write()` buat folder `/Backup` kemudian buat thread `makebak()`kemudian didalam nya akan menggunakan fungsi `execlp()`untuk mencopy file backup ke folder `/Backup`. Syntaxnya seperti berikut ini:
```
void* makebak(void*arg) {
	char root[1005] = "/home/fandipj/shift4";
    char bak[1005] = "/Backup";
	enkripsi(bak);
	char arg2[1005];
	strcpy(arg2, root);
	strcat(arg2, bak); 
	char *tmp = (char*)arg;
    char final[1005];
    strcat(final, tmp);
	char* token = strtok(tmp, "/");
	char last[1005]; 
    while (token != NULL) { 
        strcpy(last, token);
        token = strtok(NULL, "/"); 
    } 
    printf("%s\n", final);
	execlp("cp", "cp", final, arg2, NULL);
	return NULL;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	printf("--------------path_temp sebelum readdir: %s---------------\n", path_temp);
	
	enkripsi(path_temp);
	
	printf("--------------path_temp setelah readdir: %s---------------\n", path_temp);
	
	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
		printf("--------------path dalam if readdir: %s---------------\n", fpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
		printf("--------------path1 dalam else readdir: %s---------------\n", fpath);
	}

	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	int sz = strlen(path);
	if (sz > 4 && path[sz - 1] == 'p' && path[sz - 2] == 'w' && path[sz - 3] == 's' && path[sz - 4] == '.') {
		return res;
	}
	char final[1005] = "/home/fandipj/shift4";
	char root[1005] = "/Backup";
	enkripsi(root);
	strcat(final, root);
	int err = mkdir(final, 0750);
	printf("%s %d\n", final, err);
	pthread_create(&(tid[1]), NULL, &makebak, fpath);
	pthread_join(tid[1],NULL);

	return res; 
}
```
