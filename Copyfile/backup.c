switch(ot)
	{
	
		case ENUM_DIR:			
			if(it == ENUM_DIR)
			{
				strcat(dst_path,basename(src_path));
				if(is_parent_dir(src_path,dst_path))
				{
					printf("can't copy the parent directory");
					flag = false;
				}
				
				if(1 == ga.need_recursive)		
				{	
					mkdir(dst_path,0775);
				}else{
					flag = false;
					printf("omitting the directory\n");
				}
			}

			flag = prepare_copy(src_path, dst_path);
			break;
		default:
			flag = prepare_copy(src_path, dst_path);
			break;
			
	}
