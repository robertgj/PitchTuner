/** \file query.c */
/*
 * For example, run with arguments "default" or "sysdefault:Loopback"
 * or "hw:Loopback" or "hw:0,0"
 */

#include <stdio.h>
#include <errno.h>
#include <alsa/asoundlib.h>

static void query_info(snd_pcm_info_t *info)
{
  int card_no = snd_pcm_info_get_card(info);
  printf("The card number is %d\n", card_no);
  
  snd_pcm_stream_t stream = snd_pcm_info_get_stream(info);
  if (stream == SND_PCM_STREAM_CAPTURE)
    {
      printf("The stream type is capture\n");
    }
  else if (stream == SND_PCM_STREAM_PLAYBACK)
    {
      printf("The stream type is playback\n");
    }
  else
    {
      printf("The stream type is unknown\n");
    }
  
  unsigned int dev_no = snd_pcm_info_get_device(info);
  printf("The device number is %u\n", dev_no);
    
  const char * dev_id = snd_pcm_info_get_id(info);
  printf("The device id is %s\n", dev_id);
    
  const char * dev_name = snd_pcm_info_get_name(info);
  printf("The device name is %s\n", dev_name);
    
  unsigned int subdev_count = snd_pcm_info_get_subdevices_count(info);
  printf("The subdevice count is %u\n", subdev_count);
    
  unsigned int subdev_avail = snd_pcm_info_get_subdevices_avail(info);
  printf("The subdevices available is %u\n", subdev_avail);
    
  unsigned int subdev_no = snd_pcm_info_get_subdevice(info);
  printf("The subdevice number is %u\n", subdev_no);
    
  const char * subdev_name = snd_pcm_info_get_subdevice_name(info);
  printf("The subdevice name is %s\n", subdev_name); 
}

static int query_open(snd_pcm_t **pcm, snd_pcm_info_t **info,
                      char *name, snd_pcm_stream_t stream)
{
  int err = snd_pcm_open(pcm,name,stream,SND_PCM_NONBLOCK);
  if (err < 0)
    {
      fprintf (stderr, "Failed to open audio device %s (%s)\n", 
               name, snd_strerror (err));
      return err;
    }

  err = snd_pcm_info_malloc(info);
  if (err < 0)
    {
      fprintf(stderr, "Failed to allocate PCM info: %s\n",
              snd_strerror(err));
      snd_pcm_close(*pcm);
      return err;
    }
 
  err = snd_pcm_info(*pcm, *info);
  if (err < 0)
    {
      fprintf(stderr, "Failed to get PCM device info: %s\n",
              snd_strerror(err));
      snd_pcm_info_free(*info);
      snd_pcm_close(*pcm);
      return err;
    }

  return 0;
}

static void query_close(snd_pcm_t *pcm, snd_pcm_info_t *info)
{
  snd_pcm_info_free(info);
  snd_pcm_close(pcm);
}

int main(int argc, char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, "Usage : query device_name\n");
      return -1;
    }

  snd_pcm_t *pcm;
  snd_pcm_info_t *info;
  int err = query_open(&pcm,&info,argv[1],SND_PCM_STREAM_CAPTURE);
  if (err < 0)
    {
      return -1;
    }
  printf("Opened capture %s audio device\n",argv[1]);
  query_info(info);
  query_close(pcm,info);

  err = query_open(&pcm,&info,argv[1],SND_PCM_STREAM_PLAYBACK);
  if (err < 0)
    {
      return -1;
    }
  printf("Opened playback %s audio device\n",argv[1]);
  query_info(info);
  query_close(pcm,info);

  return 0;
}
