import yt_dlp
from flask import Flask, render_template, request

# Initialise the Flask App
app = Flask(__name__)

def format_bytes(size):
    """Helper function to format bytes into KB, MB, GB, etc."""
    if size is None:
        return "N/A"
    power = 1024
    n = 0
    power_labels = {0: '', 1: 'KB', 2: 'MB', 3: 'GB', 4: 'TB'}
    while size > power and n < len(power_labels) -1 :
        size /= power
        n += 1
    return f"{size:.2f} {power_labels[n]}"

@app.route('/', methods=['GET'])
def index():
    """Renders the main page with the URL input form."""
    return render_template('index.html')

@app.route('/details', methods=['POST'])
def get_details():
    """Fetches and displays video download options."""
    url = request.form['url']
    ydl_opts = {'quiet': True} # Suppress yt-dlp console output
    
    video_formats = []
    audio_formats = []
    video_title = "Unknown Title"
    error = None

    try:
        with yt_dlp.YoutubeDL(ydl_opts) as ydl:
            info = ydl.extract_info(url, download=False)
            video_title = info.get('title', 'No title')
            
            for f in info.get('formats', []):
                # We only want formats that have a direct URL
                if f.get('url'):
                    # Create a dictionary for each format
                    format_details = {
                        'url': f.get('url'),
                        'ext': f.get('ext'),
                        'filesize_str': format_bytes(f.get('filesize') or f.get('filesize_approx')),
                    }

                    # Check if it's video or audio-only
                    if f.get('vcodec') != 'none' and f.get('acodec') != 'none':
                        # It's a video format with audio
                        format_details['quality'] = f.get('resolution') or f"{f.get('height')}p"
                        video_formats.append(format_details)
                    elif f.get('vcodec') == 'none' and f.get('acodec') != 'none':
                        # It's an audio-only format
                        format_details['quality'] = f"{f.get('abr') or 0} kbps"
                        audio_formats.append(format_details)

    except Exception as e:
        print(f"Error: {e}")
        error = f"Could not process the URL. Please check if it's correct. (Error: {e})"

    return render_template('details.html', 
                           title=video_title,
                           video_formats=video_formats,
                           audio_formats=audio_formats,
                           error=error)

if __name__ == '__main__':
    # Bind to localhost on port 6969
    app.run(host='127.0.0.1', port=6969, debug=True)
