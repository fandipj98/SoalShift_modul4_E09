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
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

static const char *dirpath = "/home/fandipj/shift4";
char cipher[] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
char temp_path[1005];
int key;
pthread_t tid[1005];
int flag = 1;

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

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	printf("--------------path_temp sebelum getattr: %s---------------\n", path_temp);
	
	enkripsi(path_temp);
	
	sprintf(fpath,"%s%s",dirpath,path_temp);
	
	printf("------path_temp setelah getattr: %s------\n",path_temp);
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
		if ((strcmp(pw->pw_name, "chipset") == 0 || strcmp(pw->pw_name, "ic_controller") == 0) && strcmp(gr->gr_name, "rusak") == 0
			&&  (tmp.st_mode & 4) == 0) {
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
	int res;

	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, fpath, ts, AT_SYMLINK_NOFOLLOW);
	flag = 1;
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	printf("--------------path_temp sebelum chmod: %s---------------\n", path_temp);
	
	int sz = strlen(path_temp);
	if (strstr(path_temp, "/YOUTUBER/") != NULL && path_temp[sz - 1] == '1' 
		&& path_temp[sz - 2] == 'z' && path_temp[sz - 3] == 'i' && path_temp[sz - 4] == '.') {
    	printf("File ekstensi iz1 tidak boleh diubah permissionnya.\n");
    	return -1;
	}

	enkripsi(path_temp);
	
	printf("--------------path_temp setelah chmod: %s---------------\n", path_temp);

	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
		printf("--------------path dalam if chmod: %s---------------\n", fpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
		printf("--------------path dalam else chmod: %s---------------\n", fpath);
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

	int res;

	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_truncate(const char *path, off_t size)
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

	int res;

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
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

	int res;

	res = unlink(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	char fpath[1000],path_temp[1000];
	
	strcpy(path_temp,path);
	printf("--------------path_temp sebelum rmdir: %s---------------\n", path_temp);
	
	enkripsi(path_temp);
	
	printf("--------------path_temp setelah rmdir: %s---------------\n", path_temp);

	if(strcmp(path_temp,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
		printf("--------------path dalam if rmdir: %s---------------\n", fpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path_temp);
		printf("--------------path dalam else rmdir: %s---------------\n", fpath);
	}

	int res;

	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
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


static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.mkdir		= xmp_mkdir,
	.read		= xmp_read,
	.write		= xmp_write,
	.create     = xmp_create,
	.utimens	= xmp_utimens,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.init		= xmp_init,
	.destroy	= xmp_destroy,
};

int main(int argc, char *argv[])
{
	scanf("%d",&key);
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
