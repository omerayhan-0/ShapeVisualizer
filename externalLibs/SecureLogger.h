#ifndef SECURE_LOGGER_H
#define SECURE_LOGGER_H

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <fstream>

// Forward-declare OpenSSL type to avoid including heavy headers in .h
struct evp_cipher_ctx_st;

namespace SecureLogLib {

/**
 * @brief Temsil edilen log satırının dosyadaki konum ve boyutunu tutar.
 */
struct LogIndex {
    uint64_t offset; ///< Satırın dosyadaki başlangıç pozisyonu (bayt cinsinden)
    uint64_t length; ///< Satırın bayt uzunluğu
};


/**
 * @class CryptoProvider
 * @brief Şifreleme anahtarlarını, IV üretimini ve temel şifreleme/deşifreleme ayarlarını yönetir.
 * @details Single Responsibility Principle (SRP - Tek Sorumluluk Prensibi): 
 * Sadece kriptografik durumların güvenli yönetiminden (RAII) sorumludur.
 */
class CryptoProvider {
public:
    CryptoProvider();
    explicit CryptoProvider(const std::string& password);
    ~CryptoProvider();

    /**
     * @brief Kriptografik anahtarı (key) döndürür.
     */
    const std::vector<unsigned char>& getKey() const;

    /**
     * @brief Güvenli ve rastgele bir Initialization Vector (IV) üretir (16 bayt).
     */
    std::vector<unsigned char> generateIV() const;

private:
    std::vector<unsigned char> key_;
    void cleanMemory();
};

/**
 * @class LogWriter
 * @brief Dosyaya şifreli log yazma işlemlerinden sorumludur.
 */
class LogWriter {
public:
    explicit LogWriter(const CryptoProvider& crypto);
    ~LogWriter();

    /**
     * @brief Akıllı log yazıcı: Eğer açık bir oturum varsa kullanır, yoksa otomatik açar.
     */
    bool writeLog(const std::string& filePath, const std::string& logMessage);

    // --- Session API (yüksek frekanslı append için) ---

    /**
     * @brief Dosyayı bir kez açar, CTR sayacını hazırlar ve session'ı başlatır.
     * @details Session açıkken writeLog/appendLog çağırmayın; appendToSession kullanın.
     * @return Başarılıysa true, dosya açılamazsa false.
     */
    bool openSession(const std::string& filePath);

    /**
     * @brief Açık session'a yeni log satırı ekler. Dosya açma/kapama maliyeti yoktur.
     * @return Session açık ve yazma başarılıysa true.
     */
    bool appendToSession(const std::string& logMessage);

    /**
     * @brief Session'ı kapatır, dosyayı flusher ve kaynakları serbest bırakır.
     */
    void closeSession();

    /**
     * @brief Session'ın şu an açık olup olmadığını döndürür.
     */
    bool isSessionOpen() const { return m_sessionOpen; }

private:
    const CryptoProvider& crypto_;

    // Session state
    std::string              m_sessionFilePath;
    std::fstream             m_sessionFile;
    evp_cipher_ctx_st*       m_ctx         = nullptr;
    bool                     m_sessionOpen = false;
    uint32_t                 m_currentCrc32 = 0; // Canlı CRC32 takibi için
};

/**
 * @class LogReader
 * @brief Dosyadan şifreli log okuma ve deşifre etme işlemlerinden sorumludur.
 */
class LogReader {
public:
    explicit LogReader(const CryptoProvider& crypto);
    
    /**
     * @brief Dosyanın tamamını okur ve deşifre eder (Sadece küçük dosyalar için önerilir).
     */
    std::string readLogContent(const std::string& filePath) const;
    
    /**
     * @brief Büyük dosyalarda sadece belirli bir ofsetteki belirli bir satırı RAM dostu şekilde okur.
     */
    std::string readSingleLine(const std::string& filePath, size_t offset, size_t length) const;

private:
    const CryptoProvider& crypto_;
};

/**
 * @class LogIndexer
 * @brief Şifreli dosyayı tarayarak hızlı erişim için bir indeks (index) dosyası oluşturur.
 */
class LogIndexer {
public:
    explicit LogIndexer(const CryptoProvider& crypto);
    
    /**
     * @brief Dosyayı hızla tarayıp her bir satırın offset/uzunluk değerini '.idx' dosyasına yazar.
     * @return Dosyadaki toplam satır sayısı.
     */
    size_t buildLogIndex(const std::string& filePath) const;
    
    /**
     * @brief '.idx' dosyasından O(1) hızında sadece 16 bayt okuyarak ilgili satırın konum bilgisini döndürür.
     */
    LogIndex getLogIndex(const std::string& indexPath, size_t rowIndex) const;

private:
    const CryptoProvider& crypto_;
};


/**
 * @class SecureLogger
 * @brief Sistemin kullanıcıya bakan ana arayüzüdür (Facade Pattern).
 * @details Alt sistemlerin (Writer, Reader, Indexer, Benchmark) karmaşıklığını gizler 
 * ve önceki sistemlerle (legacy) geriye dönük tam uyumluluk sağlar.
 */
class SecureLogger {
public:
    SecureLogger();
    explicit SecureLogger(const std::string& password);
    ~SecureLogger() = default;

    // Indeksleme Islemleri
    size_t buildLogIndex(const std::string& filePath);
    LogIndex getLogIndex(const std::string& indexPath, size_t rowIndex);
    void deleteLogIndex(const std::string& filePath);

    // Okuma ve Yazma Islemleri
    std::string readSingleLine(const std::string& filePath, size_t offset, size_t length);
    bool writeLog(const std::string& filePath, const std::string& logMessage);
    std::string readLogContent(const std::string& filePath);

    // Session API Fonksiyonlari
    bool openSession(const std::string& filePath);
    bool appendToSession(const std::string& logMessage);
    void closeSession();
    bool isSessionOpen() const;

    // Benchmark (Performans Testi) Islemleri
    bool generateBenchmarkFile(const std::string& filePath, size_t size_mb, const std::string& logPattern);


private:
    std::shared_ptr<CryptoProvider> cryptoProvider_;
    std::unique_ptr<LogWriter> writer_;
    std::unique_ptr<LogReader> reader_;
    std::unique_ptr<LogIndexer> indexer_;
    std::string getIndexFilePath(const std::string& filePath) const;
};

} // namespace SecureLogLib

#endif // SECURE_LOGGER_H