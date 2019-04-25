#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>

static const char *dirpath = "/home/fandipj/shift4";
char cipher[] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
int key=17;

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

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	int sz = (int)strlen(path_temp);
	printf("------path_temp sebelum: %s------\n",path_temp);
	if (strstr(path_temp, "/YOUTUBER/") != NULL && path_temp[sz - 1] == '1' 
		&& path_temp[sz - 2] == 'z' && path_temp[sz - 3] == 'i' && path_temp[sz - 4] == '.') {
		path_temp[(int)(strlen(path_temp)) - 4] = '\0';
	}
	printf("PATH SESUDAH DITAMBAH %s\n", path_temp);
	enkripsi(path_temp);
	
	sprintf(fpath,"%s%s",dirpath,path_temp);
	
	printf("------dirpath: %s------\n",dirpath);
	printf("------path_temp setelah: %s------\n",path_temp);
	printf("------fpath: %s------\n",fpath);
	
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
		dekripsi(de->d_name);
		printf("NAMA FILE %s\n", de->d_name);
		if (strcmp(de->d_name, "c") == 0 || strcmp(de->d_name, "cc") == 0) {
			continue;
		}
		stat(fpath, &tmp);
		struct passwd *pw = getpwuid(tmp.st_uid);
		struct group *gr = getgrgid(tmp.st_gid);
		if ((strcmp(pw->pw_name, "chipset") == 0 || strcmp(pw->pw_name, "ic_controller") == 0) && strcmp(gr->gr_name, "rusak") == 0
			&&  (tmp.st_mode & 4) == 0) {
			FILE* fptr;
			char accesstime[55];
			time_t t = tmp.st_atime;
			struct tm lt;
			localtime_r(&t, &lt);

// strftime(timbuf, sizeof(timbuf), "%c", &lt);
			strftime(accesstime, 55, "%d.%m.%Y %H:%M:%S", &lt);
			char buffer[1005];
			strcat(buffer, de->d_name);
			strcat(buffer, gr->gr_name);
			strcat(buffer, pw->pw_name);
			strcat(buffer, accesstime);
			char dest[] = "miris.txt";
			enkripsi(dest);
			enkripsi(de->d_name);
			fptr = fopen(dest, "a");
			fprintf(fptr, "%s\n", buffer);
			remove(de->d_name);
			continue;
		}

		if (strstr(path, "/YOUTUBER") != NULL) {
			char tempname[1005];
			strcpy(tempname, de->d_name);
			if (S_ISREG(st.st_mode))
				strcat(tempname, ".iz1");

			// printf("NAMANYA %s\n", tempname);
			res = (filler(buf, tempname, &st, 0));
		} else
			res = (filler(buf, de->d_name, &st, 0));
		if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	printf("--------------path_temp sebelum mkdir: %s---------------\n", path_temp);
		if (strstr(path_temp, "/YOUTUBER") != NULL) {
			mode = 0000750;
		}
	
	enkripsi(path_temp);
	
	printf("--------------path_temp setelah mkdir: %s---------------\n", path_temp);

	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
		printf("--------------path dalam if mkdir: %s---------------\n", fpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
		printf("--------------path dalam else mkdir: %s---------------\n", fpath);
	}
    int res;

    res = mkdir(fpath, mode);
    if(res == -1)
        return -errno;

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	printf("--------------path_temp sebelum read: %s---------------\n", path_temp);
	
	enkripsi(path_temp);
	
	printf("--------------path_temp setelah read: %s---------------\n", path_temp);

	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
		printf("--------------path dalam if read: %s---------------\n", fpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
		printf("--------------path dalam else read: %s---------------\n", fpath);
	} 
	
	int res = 0;
  	int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	printf("--------------path_temp sebelum create: %s---------------\n", path_temp);
	
	if (strstr(path_temp, "/YOUTUBER") != NULL) {
		mode = 0000640;
	}

	enkripsi(path_temp);
	
	printf("--------------path_temp setelah create: %s---------------\n", path_temp);

	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
		printf("--------------path dalam if create: %s---------------\n", fpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
		printf("--------------path dalam else create: %s---------------\n", fpath);
	}

	// printf("FPATH akhir: %s\n", fpath);
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
	printf("--------------path_temp sebelum create: %s---------------\n", path_temp);
	
	enkripsi(path_temp);
	
	printf("--------------path_temp setelah create: %s---------------\n", path_temp);

	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
		printf("--------------path dalam if create: %s---------------\n", fpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
		printf("--------------path dalam else create: %s---------------\n", fpath);
	}
	int res;

	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, fpath, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	printf("--------------path_temp sebelum create: %s---------------\n", path_temp);
	
	enkripsi(path_temp);
	
	printf("--------------path_temp setelah create: %s---------------\n", path_temp);

	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
		printf("--------------path dalam if create: %s---------------\n", fpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
		printf("--------------path dalam else create: %s---------------\n", fpath);
	}

	int res;

	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	printf("--------------path_temp sebelum create: %s---------------\n", path_temp);
	
	enkripsi(path_temp);
	
	printf("--------------path_temp setelah create: %s---------------\n", path_temp);

	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
		printf("--------------path dalam if create: %s---------------\n", fpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
		printf("--------------path dalam else create: %s---------------\n", fpath);
	}

	int res;

	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.mkdir		= xmp_mkdir,
	.read		= xmp_read,
	.create     = xmp_create,
	.utimens	= xmp_utimens,
	.chmod		= xmp_chmod,
	.chown	= xmp_chown,
};

int main(int argc, char *argv[])
{
	//scanf("Key: %d",key);
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
