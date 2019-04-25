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

	int fd;
	fd = creat(fpath, mode);
	if (fd == -1)
		return -errno;

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
	if (res == -1)
		return -errno;

	return 0;
}
```
Untuk menambahkan file ".iz1", maka pada fungsi `xmp_readdir()` dicek dulu apakah path yang belum terenkripsi itu mengandung string `/YOUTUBER`, jika iya, maka dicek lagi, apakah sebuah folder atau file, jika file, maka path akhirnya ditambah dengan string ".iz1" sebelum di `filler()`. Kemudian pada fungsi `xmp_getattr()`, jika path yang belum terenkripsi itu mengandung string `/YOUTUBER` dan 4 character terakhirnya adalah ".iz1", maka string ".iz1" akan dibuang. Syntaxnya seperti berikut:
```
static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	int sz = (int)strlen(path_temp);
	if (strstr(path_temp, "/YOUTUBER/") != NULL && path_temp[sz - 1] == '1' 
		&& path_temp[sz - 2] == 'z' && path_temp[sz - 3] == 'i' && path_temp[sz - 4] == '.') {
		path_temp[(int)(strlen(path_temp)) - 4] = '\0';
	}
	enkripsi(path_temp);
	
	sprintf(fpath,"%s%s",dirpath,path_temp);
	
	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
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
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		struct stat tmp;
		if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
			continue;
		}
		dekripsi(de->d_name);

		if (strstr(path, "/YOUTUBER") != NULL) {
			char tempname[1005];
			strcpy(tempname, de->d_name);
			if (S_ISREG(st.st_mode))
				strcat(tempname, ".iz1");

			res = (filler(buf, tempname, &st, 0));
		} else
			res = (filler(buf, de->d_name, &st, 0));
		if(res!=0) break;
	}

	closedir(dp);
	return 0;
}
```
