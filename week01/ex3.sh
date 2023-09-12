delay()
{
  date
  sleep 3
}

mkdir ex31

delay
ls -ltr / > ex31/root.txt

delay
mkdir ex32

delay
ls -ltr ~ > ex32/home.txt

cat ex31/root.txt
cat ex32/home.txt

ls ex32
ls ex31
