
void deleteFile( const char* filename)
{
#ifdef _WIN32
	::DeleteFile(filename);
#else
	remove(filename);
#endif
}
