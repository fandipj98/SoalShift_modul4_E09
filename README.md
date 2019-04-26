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
