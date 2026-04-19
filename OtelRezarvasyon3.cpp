#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <memory>
#include <fstream> 

using namespace std;

struct MusteriBilgisi {
    string isim;
    string soyisim;
    string telefon;
    string tcKimlikNo;

    MusteriBilgisi(const string& isim, const string& soyisim, const string& telefon, const string& tc)
        : isim(isim), soyisim(soyisim),telefon(telefon), tcKimlikNo(tc) {}
};

class Oda {
protected:
    int OdaNumarasi;
    double fiyat;
    bool durum;

public:
    Oda(int numara, double temelFiyat)
        : OdaNumarasi(numara), fiyat(temelFiyat), durum(false) {}

    virtual ~Oda() {}

    virtual double fiyatHesapla() const = 0;

    inline int getOdaNumarasi() const {
        return OdaNumarasi;
    }
    inline void setDurum(bool yeniDurum) {
        durum = yeniDurum;
    }
    inline bool getDurum() const {
        return durum;
    }
};

class TekKisilikOda : public Oda {
private:
    bool ekYatakMevcut;

public:
    TekKisilikOda(int numara, double temelFiyat, bool ekYatak)
        : Oda(numara, temelFiyat), ekYatakMevcut(ekYatak) {}

    double fiyatHesapla() const override {
        return fiyat + (ekYatakMevcut ? 250.0 : 0.0);
    }

    TekKisilikOda(const TekKisilikOda& diger)
        : Oda(diger.OdaNumarasi, diger.fiyat), ekYatakMevcut(diger.ekYatakMevcut) {}
};

class CiftKisilikOda : public Oda {
private:
    bool ebeveynYatagi;

public:
    CiftKisilikOda(int numara, double temelFiyat, bool ebeveyn)
        : Oda(numara, temelFiyat), ebeveynYatagi(ebeveyn) {}

    double fiyatHesapla() const override {
        return fiyat + (ebeveynYatagi ? 400.0 : 0.0);
    }

    CiftKisilikOda(const CiftKisilikOda& diger)
        : Oda(diger.OdaNumarasi, diger.fiyat), ebeveynYatagi(diger.ebeveynYatagi) {}
};

class OdaDoluException : public exception {
public:
    const char* what() const noexcept override {
        return "Oda zaten dolu!";
    }
};

class OdaBosException : public exception {
public:
    const char* what() const noexcept override {
        return "Oda zaten bos!";
    }
};

class Odeme {
protected:
    double odemeTutari;
    string odemeTuru;

public:
    Odeme(double tutar, const string& tur)
        : odemeTutari(tutar), odemeTuru(tur) {}

    virtual ~Odeme() {}

    virtual void odemeYap() const = 0;
};

class KrediKartiOdeme : public Odeme {
private:
    string kartNumarasi;

public:
    KrediKartiOdeme(double tutar, const string& tur, const string& kartNo)
        : Odeme(tutar, tur), kartNumarasi(kartNo) {}

    void odemeYap() const override {
        cout << "Kredi karti ile " << odemeTutari << " TL odeme yapildi. Kart Numarasi: " << kartNumarasi << endl;
    }
};

class Otel;

class Rezervasyon {
    friend class Otel;

public:
    void odaDurumunuGuncelle(Otel& otel, int odaNumarasi, bool durum);
};

class Otel {
private:
    vector<unique_ptr<Oda>> odalar;
    vector<MusteriBilgisi> musteriler;

public:
    Otel() {
        for (int i = 0; i < 5; i++) {
            odalar.push_back(make_unique<TekKisilikOda>(i + 1, 500.0, false));
        }
        for (int i = 0; i < 5; i++) {
            odalar.push_back(make_unique<CiftKisilikOda>(i + 6, 800.0, false));
        }
    }

    void musteriEkle(const MusteriBilgisi& musteri) {
        musteriler.push_back(musteri);
    }

    Oda* getOda(int odaNumarasi) {
        if (odaNumarasi >= 1 && odaNumarasi <= 10)
            return odalar[odaNumarasi - 1].get();
        return nullptr;
    }

    bool tcKontrolEt(const string& tcKimlik) {
        ifstream dosya("C:\\Users\\DELL\\source\\repos\\OtelRezarvasyon3\\OtelRezarvasyon3\\MusteriBilgileri.txt");
        string line;
        while (getline(dosya, line)) {
            if (line.find(tcKimlik) != string::npos) {
                return true; 
            }
        }
        return false;
    }
};

void Rezervasyon::odaDurumunuGuncelle(Otel& otel, int odaNumarasi, bool durum) {
    try {
        Oda* oda = otel.getOda(odaNumarasi);
        if (oda) {
            if (oda->getDurum() && durum) {
                throw OdaDoluException();
            }
            else if (!oda->getDurum() && !durum) {
                throw OdaBosException();
            }
            oda->setDurum(durum);
        }
        else {
            throw runtime_error("Gecersiz oda numarasi!");
        }
    }
    catch (const exception& e) {
        cerr << "Rezervasyon hatasi: " << e.what() << endl;
        throw;
    }
}

template<typename T>
T fiyatHesapla(T temelFiyat, T ekUcret) {
    return temelFiyat + ekUcret;
}

bool tcKontrol(const string& tc) {
    if (tc.length() != 11) {
        cout << "TC Kimlik Numarasi 11 haneli olmalidir!" << endl;
        return false;
    }
    for (char c : tc) {
        if (!isdigit(c)) {
            cout << "TC Kimlik Numarasi sadece rakamlardan olusmalidir!" << endl;
            return false;
        }
    }
    return true;
}

int main() {
    Otel otel;
    try {
        string isim, soyisim, telefon, tcKimlikNo;
        int odaNumarasi, ekYatakSayisi;

        cout << "Musteri Adi: ";
        cin >> isim;
        cout << "Musteri Soyadi: ";
        cin >> soyisim;

        while (true) {
            cout << "TC Kimlik No (11 haneli): ";
            cin >> tcKimlikNo;
            if (tcKontrol(tcKimlikNo) && !otel.tcKontrolEt(tcKimlikNo)) {
                break;
            }
        }

        cout << "Telefon: ";
        cin >> telefon;

        MusteriBilgisi musteri(isim, soyisim, telefon, tcKimlikNo);
        otel.musteriEkle(musteri);
        cout << "Musteri Bilgileri: " << musteri.isim << " " << musteri.soyisim << ", TC: " << musteri.tcKimlikNo << ", Telefon: " << musteri.telefon << endl;

        
        ofstream dosya("C:\\Users\\DELL\\source\\repos\\OtelRezarvasyon3\\OtelRezarvasyon3\\MusteriBilgileri.txt", ios::app);
        if (dosya.is_open()) {
            dosya << "Musteri Adi: " << musteri.isim << endl;
            dosya << "Musteri Soyadi: " << musteri.soyisim << endl;
            dosya << "Musteri TC: " << musteri.tcKimlikNo << endl;
            dosya << "Telefon: " << musteri.telefon << endl;
            dosya << "-------------------------------" << endl;
            dosya.close();
        }
        else {
            cerr << "Dosya acilamadi!" << endl;
        }

        cout << "Tek kisilik odalar: 1-5" << endl;
        cout << "Cift kisilik odalar: 6-10" << endl;
        cout << "Rezerve etmek istediginiz oda numarasi (1-10): ";
        cin >> odaNumarasi;

        cout << "Ek yatak sayisini giriniz (0 veya 1): ";
        cin >> ekYatakSayisi;

        double ekYatakUcreti = ekYatakSayisi * 350.0;
        double odaFiyati = otel.getOda(odaNumarasi)->fiyatHesapla();

       
        double toplamFiyat = fiyatHesapla(odaFiyati, ekYatakUcreti);

        cout << "Oda Fiyati: " << odaFiyati << " TL" << endl;
        cout << "Ek Yatak Ucreti: " << ekYatakUcreti << " TL" << endl;
        cout << "Toplam Fiyat: " << toplamFiyat << " TL" << endl;

        Rezervasyon rezervasyon;
        rezervasyon.odaDurumunuGuncelle(otel, odaNumarasi, true);
        cout << "Oda " << odaNumarasi << " basariyla rezerve edildi." << endl;
        int odemeSecim;
        cout << "Odeme turunu seciniz (1: Kredi karti, 2: Diger): ";
        cin >> odemeSecim;

        if (odemeSecim == 1) {
            string kartNumarasi;
            cout << "Kart numarasi: ";
            cin >> kartNumarasi;

            KrediKartiOdeme odeme(toplamFiyat, "kredi karti", kartNumarasi);
            odeme.odemeYap();

        }
        else if (odemeSecim == 2) {
            cerr << "Gecersiz odeme turu! Oda bosaltiliyor..." << endl;
            rezervasyon.odaDurumunuGuncelle(otel, odaNumarasi, false);
            return 0;
        }
        else {
            cerr << "Gecersiz secim!" << endl;
            return 0;
        }

        Oda* oda = otel.getOda(odaNumarasi);
        if (oda) {
            if (!oda->getDurum()) {
                double odemeTutari = oda->fiyatHesapla() + ekYatakUcreti;
                cout << "Oda fiyat: " << odemeTutari << endl;
            }
            else {
                cout << "Oda dolu!" << endl;
            }
        }
        else {
            cout << "Gecersiz oda numarasi!" << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Hata: " << e.what() << endl;
    }

    return 0;
}
